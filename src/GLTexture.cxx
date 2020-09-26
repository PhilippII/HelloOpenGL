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
    glTexStorage2D(GL_TEXTURE_2D,
                   m_mipLevels,
                   GL_RGB8, // guaranteed to be supported (socalled required format)
                            // for textures but not guaranteed for renderbuffers
                   m_width,
                   m_height);

    // upload actual contents of the texture:
    std::vector<GLubyte> pix_data = makeCheckerPattern(m_width, m_height);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, // lod-level
                    0, 0, // x, y-offset
                    m_width, m_height,
                    GL_RGB, GL_UNSIGNED_BYTE,
                    pix_data.data());

    // generate content of other mipmap levels:
    glGenerateMipmap(GL_TEXTURE_2D);

    // set texture parameters:
    // glTexParameter*(..)

    // TODO:
    // 1) actually upload data to the allocated storage
    // 2) init sampling parameters
    // 3) implement move/copy-constructor/assignment
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

std::vector<GLubyte> GLTexture::makeCheckerPattern(GLsizei &width, GLsizei &height)
{
    std::array<std::vector<GLubyte>, 2> colors = {
            std::vector<GLubyte>{  0,   0,   0},
            std::vector<GLubyte>{255, 255, 255}
        };
    std::vector<GLubyte> res;
    for (GLsizei y = 0; y < height; ++y) {
        for (GLsizei x = 0; x < width; ++x) {
            constexpr mask = (1 << 4);
            int col_index = ((x & mask) ^ (y & mask) != 0) ? 1 : 0;
            std::copy(colors[col_index].begin(), colors[col_index].end(),
                      std::back_inserter(res));
        }
    }
    return res;
}
