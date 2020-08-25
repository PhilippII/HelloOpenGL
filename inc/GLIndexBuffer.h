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
          m_indexType(type), m_count(count)
    {}

    GLIndexBuffer(GLenum type, count_type count, const GLvoid* data, GLenum primitiveType, bool keepBound = true)
        : GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, count * getIndexSize(type),
                         data, GL_STATIC_DRAW, keepBound),
          m_indexType(type), m_count(count),
          m_primitiveType(primitiveType)
    {}

    GLIndexBuffer() = delete;
    GLIndexBuffer(const GLIndexBuffer& other) = delete;
    GLIndexBuffer& operator=(const GLIndexBuffer& other) = delete;

    GLIndexBuffer(GLIndexBuffer&& other)
        : GLBufferObject(std::move(other)),
          m_indexType(other.m_indexType),
          m_count(other.m_count),
          m_primitiveType(other.m_primitiveType)
    {}
    GLIndexBuffer& operator=(GLIndexBuffer&& other) {
        GLBufferObject::operator=(std::move(other));
        m_indexType = other.m_indexType;
        m_count = other.m_count;
        m_primitiveType = other.m_primitiveType;

        return *this;
    }

    ~GLIndexBuffer() {}

    GLenum getIndexType() const {
        return m_indexType;
    }

    GLenum getPrimitiveType() const {
        return m_primitiveType;
    }

    count_type getCount() const {
        return m_count;
    }

private:
    static GLuint getIndexSize(GLenum type);
    GLenum m_indexType;
    count_type m_count;
    GLenum m_primitiveType = GL_TRIANGLES;
};

#endif // GLINDEXBUFFER_H
