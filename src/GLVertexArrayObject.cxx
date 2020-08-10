#include "GLVertexArrayObject.h"



GLVertexArrayObject::GLVertexArrayObject(bool bindNow)
{
    GLCall(glGenVertexArrays(1, &m_rendererID));
    if (bindNow) {
        bind();
    }
}

GLVertexArrayObject::GLVertexArrayObject(GLVertexArrayObject&& other)
    : m_rendererID(other.m_rendererID)
    {
    other.m_rendererID = 0;
}

GLVertexArrayObject& GLVertexArrayObject::operator=(GLVertexArrayObject&& other) {
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;
    return *this;
}

GLVertexArrayObject::~GLVertexArrayObject() {
    if (m_rendererID) {
        GLCall(glDeleteVertexArrays(1, &m_rendererID));
    }
}
