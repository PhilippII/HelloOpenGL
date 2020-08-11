#ifndef GLINDEXBUFFER_H
#define GLINDEXBUFFER_H

#include <utility> // for std::move(...)

#include "GLBufferObject.h"

class GLIndexBuffer : public GLBufferObject
{
public:
    GLIndexBuffer(GLsizeiptr size, const GLvoid* data, bool keepBound = true)
        : GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW, keepBound)
    {}

    GLIndexBuffer() = delete;
    GLIndexBuffer(const GLIndexBuffer& other) = delete;
    GLIndexBuffer& operator=(const GLIndexBuffer& other) = delete;

    GLIndexBuffer(GLIndexBuffer&& other)
        : GLBufferObject(std::move(other))
    {}
    GLIndexBuffer& operator=(GLIndexBuffer&& other) {
        GLBufferObject::operator=(std::move(other));
        return *this;
    }

    ~GLIndexBuffer() {}
};

#endif // GLINDEXBUFFER_H
