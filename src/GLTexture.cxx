#include "GLTexture.h"
#include <algorithm> // for std::min(..), std::max(...), std::copy(...), ...

#include "debug_utils.h"

#include <iostream>

GLTexture::GLTexture() : m_rendererId(0),
                         m_width(512), m_height(512),
                         m_mipLevels(computeMipLevelCount(m_width, m_height)) // no mipmapping so far
{
    GLCall(glGenTextures(1, &m_rendererId));

    // first time bind also determines type of texture:
    GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererId));

    // allocate immutable storage:
    //  (immutable = immutable-format but contents may still be modified)
    GLCall(glTexStorage2D(GL_TEXTURE_2D,
                          m_mipLevels,
                          GL_RGB8, // guaranteed to be supported (socalled required format)
                                   // for textures but not guaranteed for renderbuffers
                          m_width,
                          m_height));

    // upload actual contents of the texture:
    std::vector<GLubyte> pix_data = makeCheckerPattern(m_width, m_height);
    GLCall(glTexSubImage2D(GL_TEXTURE_2D,
                           0, // lod-level
                           0, 0, // x, y-offset
                           m_width, m_height,
                           GL_RGB, GL_UNSIGNED_BYTE,
                           pix_data.data()));

    // generate content of other mipmap levels:
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));

    // set texture parameters:
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    float maxMaxAnisotropy = 1.f;
    GLCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy));
    debugDo(std::cout << "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT is " << maxMaxAnisotropy << '\n');
    GLCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(maxMaxAnisotropy, 32.f)));

    // TODO: implement move/copy-constructor/assignment
}

GLTexture::~GLTexture()
{
    GLCall(glDeleteTextures(1, &m_rendererId)); // docs.gl: "glDeleteTextures(..) silently ignores 0's [...]"
}

void GLTexture::bind(int texUnit)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + texUnit));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererId));
}

void GLTexture::unbind()
{
    myAssert(isBoundToActiveUnit());
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

bool GLTexture::isBoundToActiveUnit() const
{
    GLint currId = 0;
    GLCall(glGetIntegerv(GL_TEXTURE_BINDING_2D, &currId));
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
