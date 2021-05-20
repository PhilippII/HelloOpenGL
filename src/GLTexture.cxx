#include "GLTexture.h"
#include <algorithm> // for std::min(..), std::max(...), std::copy(...), ...
#include <iostream>
#include <array>
#include <optional>

#include "debug_utils.h"
#include "stb_image.h"

#include <utility> // std::move(..), std::exchange(..)

namespace texture_sampling_presets {
const Tex2DSamplingParams noFilter {
    GL_NEAREST, GL_NEAREST, false,
    GL_REPEAT, GL_REPEAT
};
const Tex2DSamplingParams filterPretty {
    GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true,
    GL_REPEAT, GL_REPEAT
};
}


GLTexture::GLTexture(int width, int height, GLenum internalformat, const Tex2DSamplingParams &sampParams)
{
    // reuse code shared with other constructor:
    initAndKeepBound(width, height, internalformat, sampParams);

    // do not upload any data to the allocated storage yet.
    //  -> this constructor is mainly useful for filling the texture later
    //      through a framebuffer object

    // unbind texture again:
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::GLTexture(std::filesystem::path filepath, int channels, bool sRGB, const Tex2DSamplingParams &sampParams)
{
    // load data from file:
    // m_width = m_height = 512;
    // std::vector<GLubyte> pix_data = makeCheckerPattern(m_width, m_height);
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels_in_file;
    ASSERT(1 <= channels && channels <= 4);
    unsigned char* pix_data = stbi_load(filepath.string().c_str(), &width, &height, &channels_in_file, channels);
    ASSERT(pix_data);
    DEBUG_DO(std::cout << "available channels in file " << filepath << ": " << channels_in_file << '\n');
    ASSERT(channels_in_file >= channels);

    // select internalformat based on #channels and based on whether we want to use sRGB:
    constexpr std::array<std::array<std::optional<GLenum>, 4>, 2> internalformats
               = {std::array<std::optional<GLenum>, 4>{GL_R8,        GL_RG8,       GL_RGB8,  GL_RGBA8},
                  std::array<std::optional<GLenum>, 4>{std::nullopt, std::nullopt, GL_SRGB8, GL_SRGB8_ALPHA8}};
        // GL_RGB8 and GL_SRGB8 are guaranteed to be supported (=socalled required format) for textures but
        //  not guaranteed to be supported for framebuffers.
        // the others are guaranteed to be supported (required formats) for both textures and framebuffers.
    std::optional<GLenum> myInternalformat = internalformats[static_cast<int>(sRGB)][channels - 1];
    ASSERT(myInternalformat);

    // call helper function to avoid code duplication with other constructor:
    //  1.) compute sensible number of mipmap levels and initialize
    //      the members m_width, m_height and m_mipLevels of this class
    //  2.) generate a texture object and store its id in m_rendererId
    //  3.) allocate storage for the texture with correct size and format
    //          (but do NOT yet upload the actual data to the texture storage)
    //  4.) set up the texture objects sampling parameters as defined by sampParams
    //  5.) keep the texture object bound so we can upload data to the allocated storage
    //          without having to rebind the texture first
    initAndKeepBound(width, height, *myInternalformat, sampParams);

    // upload actual data to the allocated storage:
    constexpr std::array<GLenum, 4> formats = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, // lod-level
                    0, 0, // x, y-offset
                    m_width, m_height,
                    formats[channels - 1], GL_UNSIGNED_BYTE,
                    pix_data);

    // deallocate cpu side copy of data:
    if (pix_data) {
        stbi_image_free(pix_data);
    }

    // generate content of other mipmap levels:
    if (m_mipLevels > 1) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // unbind texture again:
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::GLTexture(GLTexture &&other) noexcept
    : m_rendererId(std::exchange(other.m_rendererId, 0)),
      m_width(std::move(other.m_width)),
      m_height(std::move(other.m_height)),
      m_mipLevels(std::move(other.m_mipLevels))
{}

GLTexture& GLTexture::operator=(GLTexture &&other)
{
    if (this == &other) {
        return *this;
    }

    glDeleteTextures(1, &m_rendererId); // docs.gl: "glDeleteTextures(..) silently ignores 0's [...]"

    m_rendererId = std::exchange(other.m_rendererId, 0);
    m_width = std::move(other.m_width);
    m_height = std::move(other.m_height);
    m_mipLevels = std::move(other.m_mipLevels);

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
    ASSERT(isBoundToActiveUnit());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::initAndKeepBound(int width, int height, GLenum internalformat, const Tex2DSamplingParams& sampParams)
{
    // setup m_width, m_height and m_mipLevels:
    ASSERT(0 <= width && 0 <= height);
    m_width = width;
    m_height = height;
    m_mipLevels = (sampParams.requiresMipmap()) ? computeMipLevelCount(m_width, m_height) : 1;

    // create texture:
    glGenTextures(1, &m_rendererId);

    // first time bind also determines type of texture:
    glBindTexture(GL_TEXTURE_2D, m_rendererId);

    // allocate immutable storage:
    //  (immutable = immutable-format but contents may still be modified)
    glTexStorage2D(GL_TEXTURE_2D,
                   m_mipLevels,
                   internalformat,
                   m_width,
                   m_height);
    // I believe glTexStorage2D(..) will also take care of setting GL_TEXTURE_MAX_LEVEL
    // see: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
    //      https://www.khronos.org/opengl/wiki/Texture#Mipmap_range

    // set sampling parameters:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampParams.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampParams.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampParams.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampParams.wrap_t);
    if (sampParams.try_anisotropic_filter && GLEW_EXT_texture_filter_anisotropic) {
        float maxMaxAnisotropy = 1.f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy);
        DEBUG_DO(std::cout << "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT is " << maxMaxAnisotropy << '\n');
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(maxMaxAnisotropy, 32.f));
    }
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
    ASSERT(res.size() == colors[0].size() * width * height);
    ASSERT(colors[0].size() == colors[1].size());
    return res;
}
