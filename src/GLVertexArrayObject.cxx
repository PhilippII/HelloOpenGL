#include "GLVertexArrayObject.h"



GLVertexArrayObject::GLVertexArrayObject(bool bindNow)
{
    GLCall(glGenVertexArrays(1, &vao));
    if (bindNow) {
        bind();
    }
}

GLVertexArrayObject::GLVertexArrayObject(GLVertexArrayObject&& other)
    : vao(other.vao)
    {
    other.vao = 0;
}

GLVertexArrayObject& GLVertexArrayObject::operator=(GLVertexArrayObject&& other) {
    vao = other.vao;
    other.vao = 0;
    return *this;
}

GLVertexArrayObject::~GLVertexArrayObject() {
    if (vao) {
        GLCall(glDeleteVertexArrays(1, &vao));
    }
}
