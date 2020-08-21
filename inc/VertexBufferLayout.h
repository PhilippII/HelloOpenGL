#ifndef VERTEXBUFFERLAYOUT_H
#define VERTEXBUFFERLAYOUT_H

#include <GL/glew.h>

#include <vector>

enum class VariableType {
    // type of the actual shader variable
    // which may be different from the type of the data in the buffer
    // (e.g. we might cast integers in the buffer to float variables in the shader)
    FLOAT, NORMALIZED_FLOAT, DOUBLE, INT
};


struct VertexAttributeLayout {
    // glBindVertexBuffer   (GLintptr)
    // glVertexAttribFormat (GLuint)
    // glVertexAttribPointer(const GLvoid*)
    GLuint offset;
    GLint dimCount;
    GLenum componentType;
    VariableType castTo;

    // glVertexAttribFormat (GLuint)
    // glVertexAttribPointer(GLuint)
    GLuint location;
};


class VertexBufferLayout
{
public:
    using count_type = std::vector<VertexAttributeLayout>::size_type;

    using offset_type = GLuint; // see VertexAttributeLayout::offset

    // glBindVertexBuffer       (GLintptr)
    // glVertexArrayVertexBuffer(GLsizei)
    // glVertexAttribPointer    (GLsizei)
    using stride_type = GLsizei;

    using loc_type = GLuint; // see VertexAttributeLayout::location
    // unlike loc_type this can also hold AUTO_LOCATION:
    using signed_loc_type = GLint;

    VertexBufferLayout();
    static constexpr signed_loc_type AUTO_LOCATION = -1;
    void append(GLint dimCount, GLenum componentType, VariableType castTo, GLint location = AUTO_LOCATION);

    VertexBufferLayout& operator+=(const VertexBufferLayout& other);

    void setDefaultLocations();

    const std::vector<VertexAttributeLayout>& getAttributes() const {
        return m_attributes;
    }

    stride_type getStride() const {
        return m_stride;
    }
private:
    std::vector<VertexAttributeLayout> m_attributes;
    stride_type m_stride;

    static GLuint getAttributeSize(GLint dimCount, GLenum componentType);
};

VertexBufferLayout operator+(VertexBufferLayout a, const VertexBufferLayout& b);

#endif // VERTEXBUFFERLAYOUT_H
