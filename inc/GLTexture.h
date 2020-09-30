#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <filesystem>

class GLTexture
{
public:
    GLTexture() = delete;
    GLTexture(std::filesystem::path filepath, int channels = 3);
    // do not allow copying:
    GLTexture(const GLTexture& other) = delete;
    GLTexture& operator=(const GLTexture& other) = delete;
    // do allow moving:
    GLTexture(GLTexture&& other);
    GLTexture& operator=(GLTexture&& other);

    ~GLTexture();

    void bind(int texUnit = 0);
    void unbind();
private:
    bool isBoundToActiveUnit() const;
    static GLsizei computeMipLevelCount(GLsizei width, GLsizei height);
    static std::vector<GLubyte> makeCheckerPattern(GLsizei& width, GLsizei& height);

    GLuint m_rendererId;
    GLsizei m_width;
    GLsizei m_height;
    GLsizei m_mipLevels;
};

#endif // TEXTURE_H
