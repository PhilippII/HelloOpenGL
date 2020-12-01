#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

#include <GL/glew.h>

#include "debug_utils.h"

#include "GLVertexBuffer.h"
#include "VertexBufferLayout.h"

class GLVertexArray
{
public:
    GLVertexArray(bool bindNow = true);

    // do not allow copy:
    GLVertexArray(const GLVertexArray& other) = delete;

    GLVertexArray& operator=(const GLVertexArray& other) = delete;

    // do allow move:
    GLVertexArray(GLVertexArray&& other);

    GLVertexArray& operator=(GLVertexArray&& other);

    ~GLVertexArray();

    void addBuffer(const GLVertexBuffer& vb, const VertexBufferLayout& layout);

    void bind() {
        GLCall(glBindVertexArray(m_rendererID));
    }

    void unbind() {
        myAssert(isBound());
        GLCall(glBindVertexArray(0));
    }

    GLuint getRendererID() const {
        return m_rendererID;
    }

    bool isBound() const;

private:
    GLuint m_rendererID;
};

#endif // GLVERTEXARRAY_H
