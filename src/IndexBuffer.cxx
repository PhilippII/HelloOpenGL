#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count)
{
    myAssert(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(IndexBuffer&& other)
    : m_RendererID(other.m_RendererID), m_Count(other.m_Count)
{
    other.m_RendererID = 0;
}

IndexBuffer &IndexBuffer::operator=(IndexBuffer&& other)
{
    m_RendererID = other.m_RendererID;
    m_Count = other.m_Count;

    return *this;
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
    // docs.gl:
    // "... silently ignores 0's and names that do not correspond to existing buffer objects."
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
