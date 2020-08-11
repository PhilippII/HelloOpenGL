#include "GLVertexArray.h"



GLVertexArray::GLVertexArray(bool bindNow)
{
    GLCall(glGenVertexArrays(1, &m_rendererID));
    if (bindNow) {
        bind();
    }
}

GLVertexArray::GLVertexArray(GLVertexArray&& other)
    : m_rendererID(other.m_rendererID)
    {
    other.m_rendererID = 0;
}

GLVertexArray& GLVertexArray::operator=(GLVertexArray&& other) {
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;
    return *this;
}

GLVertexArray::~GLVertexArray() {
    if (m_rendererID) {
        GLCall(glDeleteVertexArrays(1, &m_rendererID));
    }
}

bool GLVertexArray::isBound() const
{
    GLint currVao;
    GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao));
    return (static_cast<GLuint>(currVao) == m_rendererID);
}
