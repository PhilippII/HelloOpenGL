#include "GLRenderer.h"

GLRenderer::GLRenderer()
{

}

void GLRenderer::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCall(glViewport(x, y, width, height));
}

void GLRenderer::clear(GLbitfield mask) const
{
    GLCall(glClear(mask));
}

void GLRenderer::setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCall(glClearColor(red, green, blue, alpha));
}

void GLRenderer::setClearColor(const glm::vec4 &color)
{
    GLCall(glClearColor(color.r, color.g, color.b, color.a));
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

void GLRenderer::enableBlending()
{
    GLCall(glEnable(GL_BLEND));
}

void GLRenderer::disableBlending()
{
    GLCall(glDisable(GL_BLEND));
}

void GLRenderer::setBlendFunc(GLenum sourceFactor, GLenum destFactor)
{
    GLCall(glBlendFunc(sourceFactor, destFactor));
}

void GLRenderer::setBlendEquation(GLenum mode)
{
    GLCall(glBlendEquation(mode));
}

void GLRenderer::setBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    GLCall(glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
}

void GLRenderer::setBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    GLCall(glBlendEquationSeparate(modeRGB, modeAlpha));
}

void GLRenderer::enable_framebuffer_sRGB()
{
    GLCall(glEnable(GL_FRAMEBUFFER_SRGB));
}

void GLRenderer::disable_framebuffer_sRGB()
{
    GLCall(glDisable(GL_FRAMEBUFFER_SRGB));
}

bool GLRenderer::isEnabled_framebuffer_sRGB() const
{
    GLCall(GLboolean result = glIsEnabled(GL_FRAMEBUFFER_SRGB));
    return result == GL_TRUE;
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
