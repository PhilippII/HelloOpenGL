#include "GLRenderer.h"

GLRenderer::GLRenderer()
{

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
