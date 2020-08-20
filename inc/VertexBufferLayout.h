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
    unsigned int offset;
    GLint dimCount;
    GLenum componentType;
    VariableType castTo;
    GLuint location;
};


class VertexBufferLayout
{
public:
    VertexBufferLayout();
    static constexpr GLint AUTO_LOCATION = -1;
    void append(GLint dimCount, GLenum componentType, VariableType castTo, GLint location = AUTO_LOCATION);

    VertexBufferLayout& operator+=(const VertexBufferLayout& other);

    const std::vector<VertexAttributeLayout>& getAttributes() const {
        return m_attributes;
    }

    GLsizei getStride() const {
        return m_stride;
    }
private:
    std::vector<VertexAttributeLayout> m_attributes;
    GLsizei m_stride;

    static GLuint getAttributeSize(GLint dimCount, GLenum componentType);
};

VertexBufferLayout operator+(VertexBufferLayout a, const VertexBufferLayout& b);

#endif // VERTEXBUFFERLAYOUT_H
