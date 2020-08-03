#ifndef GLVERTEXARRAYOBJECT_H
#define GLVERTEXARRAYOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug_utils.h"


class GLVertexArrayObject
{
public:
    GLVertexArrayObject();

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
        GLCall(glBindVertexArray(0));
    }

    GLuint getName() const {
        return vao;
    }

private:
    GLuint vao;
};

#endif // GLVERTEXARRAYOBJECT_H
