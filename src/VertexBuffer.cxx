#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other)
    : m_RendererID(other.m_RendererID)
{
    other.m_RendererID = 0;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer&& other)
{
    m_RendererID = other.m_RendererID;
    other.m_RendererID = 0;

    return *this;
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
    // docs.gl:
    // "... silently ignores 0's and names that do not correspond to existing buffer objects."

}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
