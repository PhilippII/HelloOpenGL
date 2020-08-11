#ifndef GLVERTEXBUFFER_H
#define GLVERTEXBUFFER_H

#include <utility> // for std::move(...)

#include "GLBufferObject.h"

class GLVertexBuffer : public GLBufferObject
{
public:
    GLVertexBuffer(GLsizeiptr size, const GLvoid* data, bool keepBound = true)
        : GLBufferObject(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW, keepBound)
    {}

    GLVertexBuffer() = delete;
    GLVertexBuffer(const GLVertexBuffer& other) = delete;
    GLVertexBuffer& operator=(const GLVertexBuffer& other) = delete;

    GLVertexBuffer(GLVertexBuffer&& other)
        : GLBufferObject(std::move(other))
    {}
    GLVertexBuffer& operator=(GLVertexBuffer&& other) {
        GLBufferObject::operator=(std::move(other));
        return *this;
    }

    ~GLVertexBuffer() {}
};

#endif // GLVERTEXBUFFER_H
