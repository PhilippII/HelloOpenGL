#include "GLTexture.h"
#include <algorithm> // for std::min(..)

GLTexture::GLTexture() : m_rendererId(0),
                         m_width(256), m_height(256),
                         m_mipLevels(computeMipLevelCount(m_width, m_height)) // no mipmapping so far
{
    glGenTextures(1, &m_rendererId);

    // first time bind also determines type of texture:
    glBindTexture(GL_TEXTURE_2D, m_rendererId);

    // allocate immutable storage:
    //  (immutable = immutable-format but contents may still be modified)
    glTexStorage2D(GL_TEXTURE_2D
                   , m_mipLevels
                   , GL_RGB8 // guaranteed to be supported (socalled required format)
                             // for textures but not guaranteed for renderbuffers
                   , m_width
                   , m_height);

    // TODO:
    // 1) actually upload data to the allocated storage
    // 2) implement move/copy-constructor/assignment
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_rendererId); // docs.gl: "glDeleteTextures(..) silently ignores 0's [...]"
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
