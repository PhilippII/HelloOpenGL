#include "GLRenderer.h"

void GLRenderer::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void GLRenderer::clear(GLbitfield mask) const
{
    glClear(mask);
}

void GLRenderer::setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    glClearColor(red, green, blue, alpha);
}

void GLRenderer::setClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderer::setClearDepth(GLdouble depth)
{
    glClearDepth(depth);
}

void GLRenderer::setClearDepth(GLfloat depth)
{
    glClearDepthf(depth);
}

void GLRenderer::setFrontFace(GLenum mode)
{
    glFrontFace(mode);
}

void GLRenderer::setCullFace(GLenum mode)
{
    glCullFace(mode);
}

void GLRenderer::enableFaceCulling()
{
    glEnable(GL_CULL_FACE);
}

void GLRenderer::disableFaceCulling()
{
    glDisable(GL_CULL_FACE);
}

void GLRenderer::enableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}

void GLRenderer::disableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}

void GLRenderer::setDepthFunc(GLenum func)
{
    glDepthFunc(func);
}

void GLRenderer::enableBlending()
{
    glEnable(GL_BLEND);
}

void GLRenderer::disableBlending()
{
    glDisable(GL_BLEND);
}

void GLRenderer::setBlendFunc(GLenum sourceFactor, GLenum destFactor)
{
    glBlendFunc(sourceFactor, destFactor);
}

void GLRenderer::setBlendEquation(GLenum mode)
{
    glBlendEquation(mode);
}

void GLRenderer::setBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void GLRenderer::setBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    glBlendEquationSeparate(modeRGB, modeAlpha);
}

void GLRenderer::enable_framebuffer_sRGB()
{
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void GLRenderer::disable_framebuffer_sRGB()
{
    glDisable(GL_FRAMEBUFFER_SRGB);
}

bool GLRenderer::isEnabled_framebuffer_sRGB() const
{
    GLboolean result = glIsEnabled(GL_FRAMEBUFFER_SRGB);
    return result == GL_TRUE;
}

void GLRenderer::draw(GLVertexArray &va, GLIndexBuffer &ib, GLShaderProgram &shaderP) const
{
    va.bind();
    ib.bind();
    shaderP.bind();
    if (ib.hasPrimitiveRestart()) {
        glPrimitiveRestartIndex(ib.getPrimitiveRestartIndex());
        glEnable(GL_PRIMITIVE_RESTART);
    }
    glDrawElements(ib.getPrimitiveType(), ib.getCount(),
                   ib.getIndexType(), nullptr);
    if (ib.hasPrimitiveRestart()) {
        glDisable(GL_PRIMITIVE_RESTART);
    }
    shaderP.unbind();
    va.unbind(); // automatically unbinds ib
}
