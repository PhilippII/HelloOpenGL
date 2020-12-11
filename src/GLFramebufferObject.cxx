#include "GLFramebufferObject.h"

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
