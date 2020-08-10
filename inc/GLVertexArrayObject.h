#ifndef GLVERTEXARRAYOBJECT_H
#define GLVERTEXARRAYOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug_utils.h"


class GLVertexArrayObject
{
public:
    GLVertexArrayObject(bool bindNow = true);

    // do not allow copy:
    GLVertexArrayObject(const GLVertexArrayObject& other) = delete;

    GLVertexArrayObject& operator=(const GLVertexArrayObject& other) = delete;

    // do allow move:
    GLVertexArrayObject(GLVertexArrayObject&& other);

    GLVertexArrayObject& operator=(GLVertexArrayObject&& other);

    ~GLVertexArrayObject();

    void bind() {
        GLCall(glBindVertexArray(m_rendererID));
    }

    void unbind() {
#ifndef NDEBUG
        GLint currVao;
        GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao));
        myAssert(static_cast<GLuint>(currVao) == m_rendererID);
#endif
        GLCall(glBindVertexArray(0));
    }

    GLuint getName() const {
        return m_rendererID;
    }

private:
    GLuint m_rendererID;
};

#endif // GLVERTEXARRAYOBJECT_H
