#ifndef GLINDEXBUFFER_H
#define GLINDEXBUFFER_H

#include <utility> // for std::move(...)
#include <optional>

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

    GLIndexBuffer(GLenum type, count_type count, const GLvoid* data, GLenum primitiveType,
                  GLuint primitiveRestartIndex, bool keepBound = true)
        : GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, count * getIndexSize(type),
                         data, GL_STATIC_DRAW, keepBound),
          m_indexType(type), m_count(count),
          m_primitiveType(primitiveType),
          m_primitiveRestartIndex(primitiveRestartIndex)
    {}

    GLIndexBuffer() = delete;
    GLIndexBuffer(const GLIndexBuffer& other) = delete;
    GLIndexBuffer& operator=(const GLIndexBuffer& other) = delete;

    GLIndexBuffer(GLIndexBuffer&& other) noexcept = default; // most of the work is done by parent class'
                                                             // move constructor anyway
    GLIndexBuffer& operator=(GLIndexBuffer&& other) = default;
    //  warning: moved from object must be destroyed or
    //           assigned to before being used again
    //  TODO: how to make move-assignment noexcept? (how to make parent-class' move-assignment noexcept?)

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

    bool hasPrimitiveRestart() const {
        return m_primitiveRestartIndex.has_value();
    }

    GLuint getPrimitiveRestartIndex() const {
        ASSERT(m_primitiveRestartIndex.has_value());
        return *m_primitiveRestartIndex;
    }

private:
    static GLuint getIndexSize(GLenum type);
    GLenum m_indexType;
    count_type m_count;
    GLenum m_primitiveType = GL_TRIANGLES;
    std::optional<GLuint> m_primitiveRestartIndex = {}; // primitive restart disabled by default
};

#endif // GLINDEXBUFFER_H
