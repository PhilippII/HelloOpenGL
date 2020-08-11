#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug_utils.h"


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

    void bind() {
        GLCall(glBindVertexArray(m_rendererID));
    }

    void unbind() {
        myAssert(isBound());
        GLCall(glBindVertexArray(0));
    }

    GLuint getName() const {
        return m_rendererID;
    }

    bool isBound() const;

private:
    GLuint m_rendererID;
};

#endif // GLVERTEXARRAY_H
