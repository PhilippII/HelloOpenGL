#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

class GLTexture
{
public:
    GLTexture();
    ~GLTexture();
private:
    static GLsizei computeMipLevelCount(GLsizei width, GLsizei height);
    static std::vector<GLubyte> makeCheckerPattern(GLsizei& width, GLsizei& height);
    GLuint m_rendererId;
    GLsizei m_width;
    GLsizei m_height;
    GLsizei m_mipLevels;
};

#endif // TEXTURE_H
