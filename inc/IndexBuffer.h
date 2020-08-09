#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_Count;
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer& other) = delete;
    IndexBuffer& operator=(const IndexBuffer& other) = delete;
    IndexBuffer(IndexBuffer&& other);
    IndexBuffer& operator=(IndexBuffer&& other);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    unsigned int GetCount() const { return m_Count; }
};

#endif // INDEXBUFFER_H
