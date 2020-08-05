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
        GLCall(glBindVertexArray(vao));
    }

    void unbind() {
#ifndef NDEBUG
        GLint currVao;
        GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currVao));
        myAssert(static_cast<GLuint>(currVao) == vao);
#endif
        GLCall(glBindVertexArray(0));
    }

    GLuint getName() const {
        return vao;
    }

private:
    GLuint vao;
};

#endif // GLVERTEXARRAYOBJECT_H
