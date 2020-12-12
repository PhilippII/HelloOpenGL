#include "GLVertexArray.h"

#include <iostream>

GLVertexArray::GLVertexArray(bool bindNow)
{
    glGenVertexArrays(1, &m_rendererID);
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
        glDeleteVertexArrays(1, &m_rendererID);
    }
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;
    return *this;
}

GLVertexArray::~GLVertexArray() {
    if (m_rendererID) {
        glDeleteVertexArrays(1, &m_rendererID);
    }
}

void GLVertexArray::addBuffer(const GLVertexBuffer &vb, const VertexBufferLayout &layout)
{
    bind();
    const GLuint bindingIndex = 0;
    glBindVertexBuffer(bindingIndex, vb.getRendererID(), 0, layout.getStride());
    const std::vector<VertexAttributeLayout>& attributes = layout.getAttributes();
    for (auto& attr : attributes) {
        if (!attr.location) {
            std::cout << "warning attribute " << attr.name << " has no location. VAO will ignore this attribute.\n";
            continue;
        }
        glEnableVertexAttribArray(*attr.location);
        ASSERT(VertexBufferLayout::isValidCast(attr.componentType, attr.castTo));
        switch (attr.castTo) {
          case VariableType::NORMALIZED_FLOAT:
          case VariableType::FLOAT:
            glVertexAttribFormat(*attr.location, attr.dimCount,
                                 attr.componentType, (attr.castTo == VariableType::NORMALIZED_FLOAT),
                                 attr.offset);
            break;
          case VariableType::INT:
            glVertexAttribIFormat(*attr.location, attr.dimCount,
                                  attr.componentType,
                                  attr.offset);
            break;
          case VariableType::DOUBLE:
            glVertexAttribLFormat(*attr.location, attr.dimCount,
                                  attr.componentType,
                                  attr.offset);
            break;
          default:
            ASSERT(false);
            break;
        }
        glVertexAttribBinding(*attr.location, bindingIndex);
    }
}

bool GLVertexArray::isBound() const
{
    GLint currVao;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao);
    return (static_cast<GLuint>(currVao) == m_rendererID);
}
