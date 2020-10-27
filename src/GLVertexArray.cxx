#include "GLVertexArray.h"

#include <iostream>

GLVertexArray::GLVertexArray(bool bindNow)
{
    GLCall(glGenVertexArrays(1, &m_rendererID));
    if (bindNow) {
        bind();
    }
}

GLVertexArray::GLVertexArray(GLVertexArray&& other)
    : m_rendererID(other.m_rendererID)
    {
    other.m_rendererID = 0;
}

GLVertexArray& GLVertexArray::operator=(GLVertexArray&& other) {
    if (this == &other) {
        return *this;
    }
    if (m_rendererID) {
        GLCall(glDeleteVertexArrays(1, &m_rendererID));
    }
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;
    return *this;
}

GLVertexArray::~GLVertexArray() {
    if (m_rendererID) {
        GLCall(glDeleteVertexArrays(1, &m_rendererID));
    }
}

void GLVertexArray::addBuffer(const GLVertexBuffer &vb, const VertexBufferLayout &layout)
{
    bind();
    const GLuint bindingIndex = 0;
    GLCall(glBindVertexBuffer(bindingIndex, vb.getRendererID(), 0, layout.getStride()));
    const std::vector<VertexAttributeLayout>& attributes = layout.getAttributes();
    for (auto& attr : attributes) {
        if (!attr.location) {
            std::cout << "warning attribute " << attr.name << " has no location. VAO will ignore this attribute.\n";
            continue;
        }
        GLCall(glEnableVertexAttribArray(*attr.location));
        myAssert(VertexBufferLayout::isValidCast(attr.componentType, attr.castTo));
        switch (attr.castTo) {
          case VariableType::NORMALIZED_FLOAT:
          case VariableType::FLOAT:
            GLCall(glVertexAttribFormat(*attr.location, attr.dimCount,
                                        attr.componentType, (attr.castTo == VariableType::NORMALIZED_FLOAT),
                                        attr.offset));
            break;
          case VariableType::INT:
            GLCall(glVertexAttribIFormat(*attr.location, attr.dimCount,
                                         attr.componentType,
                                         attr.offset));
            break;
          case VariableType::DOUBLE:
            GLCall(glVertexAttribLFormat(*attr.location, attr.dimCount,
                                         attr.componentType,
                                         attr.offset));
            break;
          default:
            myAssert(false);
            break;
        }
        GLCall(glVertexAttribBinding(*attr.location, bindingIndex));
    }
}

bool GLVertexArray::isBound() const
{
    GLint currVao;
    GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao));
    return (static_cast<GLuint>(currVao) == m_rendererID);
}
