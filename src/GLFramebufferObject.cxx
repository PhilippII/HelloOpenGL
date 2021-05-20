#include "GLFramebufferObject.h"

#include "debug_utils.h"

GLFramebufferObject::GLFramebufferObject()
{
    glGenFramebuffers(1, &m_rendererId);
}


GLFramebufferObject::GLFramebufferObject(GLFramebufferObject&& other) noexcept
    : m_rendererId(std::exchange(other.m_rendererId, 0))
{}

GLFramebufferObject &GLFramebufferObject::operator=(GLFramebufferObject&& other)
{
    if (this == &other) {
        return *this;
    }

    glDeleteFramebuffers(1, &m_rendererId);

    m_rendererId = std::exchange(other.m_rendererId, 0);

    return *this;
}


GLFramebufferObject::~GLFramebufferObject()
{
    glDeleteFramebuffers(1, &m_rendererId);
    // "The name zero is reserved by the GL and is silently ignored,
    //  should it occur in framebuffers, as are other unused names."
    // docs.gl
}

void GLFramebufferObject::bind(GLenum target)
{
    glBindFramebuffer(target, m_rendererId);
}

void GLFramebufferObject::unbind(GLenum target)
{
    ASSERT(!isOtherFBObound(target)); // - do NOT allow unbinding a different fbo B
                                        //   by calling A.unbind() on a fbo A != B.
                                        // - do allow calling this method when the default framebuffer
                                        //   is bound (in this case it has no effect)
                                        // - do allow unbinding fbo A by calling A.unbind()
    glBindFramebuffer(target, 0);
}

void GLFramebufferObject::setDrawBuffers(gsl::span<GLenum> drawBuffers)
{
    ASSERT(isBound(GL_DRAW_FRAMEBUFFER));
    // docs.gl: "For glDrawBuffers, the framebuffer object that is bound
    //           to the GL_DRAW_FRAMEBUFFER binding will be used."
    ASSERT(drawBuffers.size() <= getMaxDrawBuffers());
    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
}

void GLFramebufferObject::attachTexture(GLenum attachment, const GLTexture &texture, GLint mipLevel)
{
    ASSERT(isBound(GL_DRAW_FRAMEBUFFER));
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, texture.getRendererId(), mipLevel);
}

void GLFramebufferObject::unattachTexture(GLenum attachment)
{
    ASSERT(isBound(GL_DRAW_FRAMEBUFFER));
    // TODO: is this also correct for other texture types?
    //          e.g. GL_TEXTURE_3D
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, 0, 0);

}

GLenum GLFramebufferObject::checkFramebufferStatus() const
{
    ASSERT(isBound(GL_DRAW_FRAMEBUFFER));
    return glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
}

GLuint GLFramebufferObject::getMaxDrawBuffers()
{
    GLint max = 0;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max);
    return static_cast<GLuint>(max);
}

GLuint GLFramebufferObject::getBoundFramebuffer(GLenum target)
{
    GLenum pname = 0;
    switch (target) {
    case GL_READ_FRAMEBUFFER:
        pname = GL_READ_FRAMEBUFFER_BINDING;
        break;
    case GL_DRAW_FRAMEBUFFER:
    case GL_FRAMEBUFFER:
        pname = GL_DRAW_FRAMEBUFFER_BINDING;
        break;
    default:
        ASSERT(false);
        break;
    }
    GLint bound_fbo = 0;
    glGetIntegerv(pname, &bound_fbo);
    return static_cast<GLuint>(bound_fbo);
}



bool GLFramebufferObject::isBound(GLenum target) const
{

    return m_rendererId == getBoundFramebuffer(target);
}

bool GLFramebufferObject::isOtherFBObound(GLenum target) const
{
    GLuint boundFB = getBoundFramebuffer(target);
    return boundFB != m_rendererId && boundFB != 0;
}
