#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer
{
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    VertexBuffer() = delete;
    VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;
    VertexBuffer(VertexBuffer&& other);
    VertexBuffer& operator=(VertexBuffer&& other);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};

#endif // VERTEXBUFFER_H
