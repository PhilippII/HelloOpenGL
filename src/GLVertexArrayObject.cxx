#include "GLVertexArrayObject.h"



GLVertexArrayObject::GLVertexArrayObject()
{
    GLCall(glGenVertexArrays(1, &vao));
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
