#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <vector>
#include <filesystem>


struct Tex2DSamplingParams {
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
    extern const Tex2DSamplingParams noFilter;
    extern const Tex2DSamplingParams filterPretty;
}


class GLTexture
{
public:
    GLTexture() = delete;
    GLTexture(int width, int height, GLenum internalformat, const Tex2DSamplingParams &sampParams);
    GLTexture(std::filesystem::path filepath, int channels = 3, bool sRGB = false,
              const Tex2DSamplingParams& sampParams = texture_sampling_presets::filterPretty);
    // do not allow copying:
    GLTexture(const GLTexture& other) = delete;
    GLTexture& operator=(const GLTexture& other) = delete;
    // do allow moving:
    GLTexture(GLTexture&& other) noexcept;
    GLTexture& operator=(GLTexture&& other);
    //  warning: moved from object must be destroyed or
    //           assigned to before being used again
    //  TODO: how to make move-assignment noexcept? (is glDeleteTextures(..) noexcept?)

    ~GLTexture();

    void bind(int texUnit = 0);
    void unbind();

    GLuint getRendererId() const {
        return m_rendererId;
    }
private:
    void initAndKeepBound(int width, int height, GLenum internalformat, const Tex2DSamplingParams &sampParams);
    bool isBoundToActiveUnit() const;
    static GLsizei computeMipLevelCount(GLsizei width, GLsizei height);
    static std::vector<GLubyte> makeCheckerPattern(GLsizei& width, GLsizei& height);

    GLuint m_rendererId;
    GLsizei m_width;
    GLsizei m_height;
    GLsizei m_mipLevels;
};

#endif // TEXTURE_H
