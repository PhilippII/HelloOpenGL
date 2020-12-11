#ifndef GLFRAMEBUFFEROBJECT_H
#define GLFRAMEBUFFEROBJECT_H

#include <GL/glew.h>
#include <gsl/gsl> // for gsl::span<>

#include "GLTexture.h"

class GLFramebufferObject
{
public:
    GLFramebufferObject();
    // do not allow copy:
    GLFramebufferObject(const GLFramebufferObject& other) = delete;
    GLFramebufferObject& operator=(const GLFramebufferObject& other) = delete;
    // do allow move:
    GLFramebufferObject(GLFramebufferObject&& other);
    GLFramebufferObject& operator=(GLFramebufferObject&& other);

    ~GLFramebufferObject();

    void bind(GLenum target = GL_FRAMEBUFFER);

    void setDrawBuffers(gsl::span<GLenum> drawBuffers);

    void attachTexture(GLenum attachment, const GLTexture& texture, GLint mipLevel = 0);
    void unattachTexture(GLenum attachment);

    GLenum checkFramebufferStatus() const;

    static GLuint getMaxDrawBuffers();
private:
    bool isBound(GLenum target = GL_DRAW_FRAMEBUFFER) const;
    GLuint m_rendererId;
};

#endif // GLFRAMEBUFFEROBJECT_H
