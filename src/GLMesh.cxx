#include "GLMesh.h"

const GLuint GLMesh::bindingIndex = 0;


GLMesh::GLMesh(const VertexBufferLayout& layout, const GLvoid* vertexData, GLsizei vertexCount,
               GLenum indicesType, const GLvoid* indexData, GLsizei indexCount,
               bool keepBound)
    : m_vao(),
      m_vb(vertexCount * layout.getStride(), vertexData, false),
      m_ib(indicesType, indexCount, indexData),
      m_vertexCount(vertexCount)
{
    GLCall(glBindVertexBuffer(bindingIndex, m_vb.getName(), 0, layout.getStride()));
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
    // ibo bound automatically in constructor already
    if (!keepBound) {
        m_vao.unbind();
    }
}

GLMesh::GLMesh(GLMesh&& other)
    : m_vao(std::move(other.m_vao)),
      m_vb(std::move(other.m_vb)), m_ib(std::move(other.m_ib)),
      m_vertexCount(other.m_vertexCount)
{}

GLMesh& GLMesh::operator=(GLMesh&& other) {
    m_vao = std::move(other.m_vao);
    m_vb = std::move(other.m_vb);
    m_ib = std::move(other.m_ib);
    m_vertexCount = other.m_vertexCount;

    return *this;
}

GLMesh::~GLMesh() {
    // destructors of members are called automatically
}


bool GLMesh::isFloatingPoint(GLenum componentType) {
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

