#include "GLTexture.h"
#include <algorithm> // for std::min(..), std::max(...), std::copy(...), ...
#include <iostream>
#include <array>
#include <optional>

#include "debug_utils.h"
#include "stb_image.h"


namespace texture_sampling_presets {
const Texture2DSamplingParameters noFilter {
    GL_NEAREST, GL_NEAREST, false,
    GL_REPEAT, GL_REPEAT
};
const Texture2DSamplingParameters filterPretty {
    GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true,
    GL_REPEAT, GL_REPEAT
};
}


GLTexture::GLTexture(std::filesystem::path filepath, int channels, bool sRGB)
    : m_rendererId(0),
      m_width(0), m_height(0),
      m_mipLevels(0)
{
    // create texture:
    glGenTextures(1, &m_rendererId);

    // first time bind also determines type of texture:
    glBindTexture(GL_TEXTURE_2D, m_rendererId);

    // load data from file:
    // m_width = m_height = 512;
    // std::vector<GLubyte> pix_data = makeCheckerPattern(m_width, m_height);
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels_in_file;
    myAssert(1 <= channels && channels <= 4);
    unsigned char* pix_data = stbi_load(filepath.string().c_str(), &width, &height, &channels_in_file, channels);
    myAssert(pix_data);
    m_width = width;
    m_height = height;
    m_mipLevels = computeMipLevelCount(m_width, m_height);
    debugDo(std::cout << "available channels in file " << filepath << ": " << channels_in_file << '\n');
    myAssert(channels_in_file >= channels);

    // allocate immutable storage:
    //  (immutable = immutable-format but contents may still be modified)
    constexpr std::array<std::array<std::optional<GLenum>, 4>, 2> internalformats
               = {std::array<std::optional<GLenum>, 4>{GL_R8,        GL_RG8,       GL_RGB8,  GL_RGBA8},
                  std::array<std::optional<GLenum>, 4>{std::nullopt, std::nullopt, GL_SRGB8, GL_SRGB8_ALPHA8}};
    // GL_RGB8 and GL_SRGB8 are guaranteed to be supported (=socalled required format) for textures but
    //  not guaranteed to be supported for framebuffers.
    // the others are guaranteed to be supported (required formats) for both textures and framebuffers.
    myAssert(internalformats[static_cast<int>(sRGB)][channels - 1]);
    glTexStorage2D(GL_TEXTURE_2D,
                   m_mipLevels,
                   *(internalformats[static_cast<int>(sRGB)][channels - 1]),
                   m_width,
                   m_height);
    // I believe glTexStorage2D(..) will also take care of setting GL_TEXTURE_MAX_LEVEL
    // see: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture

    // upload actual data to the allocated storage:
    constexpr std::array<GLenum, 4> formats = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, // lod-level
                    0, 0, // x, y-offset
                    m_width, m_height,
                    formats[channels - 1], GL_UNSIGNED_BYTE,
                    pix_data);
                    // pix_data.data())); // use this for the checker pattern version

    // deallocate cpu side copy of data:
    if (pix_data) {
        stbi_image_free(pix_data);
    }

    // generate content of other mipmap levels:
    glGenerateMipmap(GL_TEXTURE_2D);

    // set sampling parameters:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxMaxAnisotropy = 1.f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy);
        debugDo(std::cout << "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT is " << maxMaxAnisotropy << '\n');
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(maxMaxAnisotropy, 32.f));
    }

    // unbind texture again:
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::GLTexture(GLTexture &&other)
    : m_rendererId(other.m_rendererId),
      m_width(other.m_width), m_height(other.m_height),
      m_mipLevels(other.m_mipLevels)
{
    other.m_rendererId = 0;
}

GLTexture& GLTexture::operator=(GLTexture &&other)
{
    if (this == &other) {
        return *this;
    }
    m_rendererId = other.m_rendererId;
    other.m_rendererId = 0;
    m_width = other.m_width;
    m_height = other.m_height;
    m_mipLevels = other.m_mipLevels;
    return *this;
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_rendererId); // docs.gl: "glDeleteTextures(..) silently ignores 0's [...]"
}

void GLTexture::bind(int texUnit)
{
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_rendererId);
}

void GLTexture::unbind()
{
    myAssert(isBoundToActiveUnit());
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool GLTexture::isBoundToActiveUnit() const
{
    GLint currId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currId);
    return m_rendererId == static_cast<GLuint>(currId);
}


GLsizei GLTexture::computeMipLevelCount(GLsizei width, GLsizei height)
{
    auto size = std::max(width, height);
    GLsizei level;
    for (level = 0; size >> level; ++level) {
        // std::cout << "mipmap-level " << level << " has size "
        //           << (size >> level) << '\n';
    }
    return level; // return first level where (size >> level) would be zero
}

std::vector<GLubyte> GLTexture::makeCheckerPattern(GLsizei &width, GLsizei &height)
{
    std::array<std::vector<GLubyte>, 2> colors = {
            std::vector<GLubyte>{  0,   0,   0},
            std::vector<GLubyte>{255, 255, 255}
        };
    std::vector<GLubyte> res;
    for (GLsizei y = 0; y < height; ++y) {
        for (GLsizei x = 0; x < width; ++x) {
            int col_index = !!((x ^ y) & (1 << 3)); // !! converts to bool, after that
                                                    // implicitly convert to int again
            std::copy(colors[col_index].begin(), colors[col_index].end(),
                      std::back_inserter(res));
        }
    }
    myAssert(res.size() == colors[0].size() * width * height);
    myAssert(colors[0].size() == colors[1].size());
    return res;
}
