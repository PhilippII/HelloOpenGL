#include "GLRenderer.h"

GLRenderer::GLRenderer()
{

}

void GLRenderer::clear(GLbitfield mask) const
{
    GLCall(glClear(mask));
}

void GLRenderer::setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCall(glClearColor(red, green, blue, alpha));
}

void GLRenderer::setClearDepth(GLdouble depth)
{
    GLCall(glClearDepth(depth));
}

void GLRenderer::setClearDepth(GLfloat depth)
{
    GLCall(glClearDepthf(depth));
}

void GLRenderer::draw(GLVertexArray &va, GLIndexBuffer &ib, GLShaderProgram &shaderP) const
{
    va.bind();
    ib.bind();
    shaderP.bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(),
                          ib.getType(), nullptr));
    shaderP.unbind();
    va.unbind(); // automatically unbinds ib
}
