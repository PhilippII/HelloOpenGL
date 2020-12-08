#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <vector>
#include <filesystem>


struct Texture2DSamplingParameters {
    GLint mag_filter = GL_LINEAR;
    GLint min_filter = GL_NEAREST_MIPMAP_LINEAR;
    bool try_anisotropic_filter = false;

    GLint wrap_s = GL_REPEAT;
    GLint wrap_t = GL_REPEAT;

    bool requiresMipmap() const {
        return min_filter == GL_NEAREST_MIPMAP_NEAREST
                || min_filter == GL_LINEAR_MIPMAP_NEAREST
                || min_filter == GL_NEAREST_MIPMAP_LINEAR
                || min_filter == GL_LINEAR_MIPMAP_LINEAR;
    }
};

namespace texture_sampling_presets {
    extern const Texture2DSamplingParameters noFilter;
    extern const Texture2DSamplingParameters filterPretty;
}


class GLTexture
{
public:
    GLTexture() = delete;
    GLTexture(std::filesystem::path filepath, int channels = 3, bool sRGB = false);
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
