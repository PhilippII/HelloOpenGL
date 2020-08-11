#include "GLMesh.h"

const GLuint GLMesh::bindingIndex = 0;


GLMesh::GLMesh(const VertexBufferLayout& layout, const GLvoid* vertexData, GLsizei vertexCount,
               GLenum indicesType, const GLvoid* indexData, GLsizei indexCount,
               bool keepBound)
    : m_vb(vertexCount * layout.getStride(), vertexData, false),
      m_va(),
      m_ib(indicesType, indexCount, indexData),
      m_vertexCount(vertexCount)
{
    m_va.addBuffer(m_vb, layout);
    // m_va and m_ib bound in their constructors by default
    if (!keepBound) {
        // m_ib.unbind(); unbound with m_va automatically
        m_va.unbind();
    }
}

GLMesh::GLMesh(GLMesh&& other)
    : m_vb(std::move(other.m_vb)),
      m_va(std::move(other.m_va)),
      m_ib(std::move(other.m_ib)),
      m_vertexCount(other.m_vertexCount)
{}

GLMesh& GLMesh::operator=(GLMesh&& other) {
    m_vb = std::move(other.m_vb);
    m_va = std::move(other.m_va);
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

