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

void GLRenderer::setFrontFace(GLenum mode)
{
    GLCall(glFrontFace(mode));
}

void GLRenderer::setCullFace(GLenum mode)
{
    GLCall(glCullFace(mode));
}

void GLRenderer::enableFaceCulling()
{
    GLCall(glEnable(GL_CULL_FACE));
}

void GLRenderer::disableFaceCulling()
{
    GLCall(glDisable(GL_CULL_FACE));
}

void GLRenderer::enableDepthTest()
{
    GLCall(glEnable(GL_DEPTH_TEST));
}

void GLRenderer::disableDepthTest()
{
    GLCall(glDisable(GL_DEPTH_TEST));
}

void GLRenderer::setDepthFunc(GLenum func)
{
    GLCall(glDepthFunc(func));
}

void GLRenderer::draw(GLVertexArray &va, GLIndexBuffer &ib, GLShaderProgram &shaderP) const
{
    va.bind();
    ib.bind();
    shaderP.bind();
    if (ib.hasPrimitiveRestart()) {
        GLCall(glPrimitiveRestartIndex(ib.getPrimitiveRestartIndex()));
        GLCall(glEnable(GL_PRIMITIVE_RESTART));
    }
    GLCall(glDrawElements(ib.getPrimitiveType(), ib.getCount(),
                          ib.getIndexType(), nullptr));
    if (ib.hasPrimitiveRestart()) {
        GLCall(glDisable(GL_PRIMITIVE_RESTART));
    }
    shaderP.unbind();
    va.unbind(); // automatically unbinds ib
}
