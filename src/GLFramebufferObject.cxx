#include "GLFramebufferObject.h"

#include "debug_utils.h"

GLFramebufferObject::GLFramebufferObject()
{
    glGenFramebuffers(1, &m_rendererId);
}


GLFramebufferObject::GLFramebufferObject(GLFramebufferObject&& other)
    : m_rendererId(other.m_rendererId)
{
    other.m_rendererId = 0;
}

GLFramebufferObject &GLFramebufferObject::operator=(GLFramebufferObject&& other)
{
    if (this == &other) {
        return *this;
    }
    glDeleteFramebuffers(1, &m_rendererId);
    m_rendererId = other.m_rendererId;
    other.m_rendererId = 0;
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

void GLFramebufferObject::setDrawBuffers(gsl::span<GLenum> drawBuffers)
{
    myAssert(isBound(GL_DRAW_FRAMEBUFFER));
    // docs.gl: "For glDrawBuffers, the framebuffer object that is bound
    //           to the GL_DRAW_FRAMEBUFFER binding will be used."
    myAssert(drawBuffers.size() <= getMaxDrawBuffers());
    glDrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void GLFramebufferObject::attachTexture(GLenum attachment, const GLTexture &texture, GLint mipLevel)
{
    myAssert(isBound(GL_DRAW_FRAMEBUFFER));
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, texture.getRendererId(), mipLevel);
}

void GLFramebufferObject::unattachTexture(GLenum attachment)
{
    myAssert(isBound(GL_DRAW_FRAMEBUFFER));
    // TODO: is this also correct for other texture types?
    //          e.g. GL_TEXTURE_3D
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, 0, 0);

}

GLuint GLFramebufferObject::getMaxDrawBuffers()
{
    GLint max = 0;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max);
    return static_cast<GLuint>(max);
}

bool GLFramebufferObject::isBound(GLenum target) const
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
        myAssert(false);
    }
    GLint bound_fbo = 0;
    glGetIntegerv(pname, &bound_fbo);
    return m_rendererId == static_cast<GLuint>(bound_fbo);
}
