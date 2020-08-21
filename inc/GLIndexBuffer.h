#ifndef GLINDEXBUFFER_H
#define GLINDEXBUFFER_H

#include <utility> // for std::move(...)

#include "GLBufferObject.h"

class GLIndexBuffer : public GLBufferObject
{
public:
    // glDrawElements(GLsizei)
    using count_type = GLsizei;

    GLIndexBuffer(GLenum type, count_type count, const GLvoid* data, bool keepBound = true)
        : GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, count * getIndexSize(type),
                         data, GL_STATIC_DRAW, keepBound),
          m_type(type), m_count(count)
    {}

    GLIndexBuffer() = delete;
    GLIndexBuffer(const GLIndexBuffer& other) = delete;
    GLIndexBuffer& operator=(const GLIndexBuffer& other) = delete;

    GLIndexBuffer(GLIndexBuffer&& other)
        : GLBufferObject(std::move(other)),
          m_type(other.m_type),
          m_count(other.m_count)
    {}
    GLIndexBuffer& operator=(GLIndexBuffer&& other) {
        GLBufferObject::operator=(std::move(other));
        m_type = other.m_type;
        m_count = other.m_count;

        return *this;
    }

    ~GLIndexBuffer() {}

    GLenum getType() const {
        return m_type;
    }

    count_type getCount() const {
        return m_count;
    }

private:
    static GLuint getIndexSize(GLenum type);
    GLenum m_type;
    count_type m_count;
};

#endif // GLINDEXBUFFER_H
