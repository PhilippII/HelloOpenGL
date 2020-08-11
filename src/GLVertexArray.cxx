#include "GLVertexArray.h"



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
    GLCall(glBindVertexBuffer(bindingIndex, vb.getName(), 0, layout.getStride()));
    const std::vector<VertexAttributeLayout>& attributes = layout.getAttributes();
    for (auto& attr : attributes) {
        GLCall(glEnableVertexAttribArray(attr.location));
        bool normalized = false;
        switch (attr.castTo) {
          case VariableType::FLOAT:
          case VariableType::NORMALIZED_FLOAT:
            normalized = (attr.castTo == VariableType::NORMALIZED_FLOAT)
                         && !isFloatingPoint(attr.componentType);
            GLCall(glVertexAttribFormat(attr.location, attr.dimCount,
                                        attr.componentType, normalized,
                                        attr.offset));
            break;
          case VariableType::DOUBLE:
            GLCall(glVertexAttribIFormat(attr.location, attr.dimCount,
                                         attr.componentType,
                                         attr.offset));
            break;
          case VariableType::INT:
            GLCall(glVertexAttribLFormat(attr.location, attr.dimCount,
                                         attr.componentType,
                                         attr.offset));
            break;
          default:
            myAssert(false);
            break;
        }
        GLCall(glVertexAttribBinding(attr.location, bindingIndex));
    }
}

bool GLVertexArray::isBound() const
{
    GLint currVao;
    GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao));
    return (static_cast<GLuint>(currVao) == m_rendererID);
}


bool GLVertexArray::isFloatingPoint(GLenum componentType) { // TODO: delete this from class GLMesh
    switch (componentType) {
      case GL_HALF_FLOAT:
      case GL_FLOAT:
      case GL_DOUBLE:
      case GL_FIXED:
        return true;
      default:
        return false;
    }
}
