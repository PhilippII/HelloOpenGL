#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLVertexArray.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLShaderProgram.h"

class GLRenderer
{
public:
    GLRenderer();

    void clear(GLbitfield mask) const;

    void setClearColor(GLfloat red = 0.f, GLfloat green = 0.f, GLfloat blue = 0.f, GLfloat alpha = 0.f);

    void setClearDepth(GLdouble depth = 1.);

    void setClearDepth(GLfloat depth = 1.f);

    // TODO: setClearStencil(...)

    // TODO: getClear(Color|Depth|Stencil)(...)

    void setFrontFace(GLenum mode = GL_CCW);

    void setCullFace(GLenum mode = GL_BACK);

    void enableFaceCulling();

    void disableFaceCulling();

    void enableDepthTest();

    // khronos.org:
    // "When depth testing is disabled, writes to the depth buffer are also disabled."
    void disableDepthTest();

    void setDepthFunc(GLenum func = GL_LESS);

    void draw(GLVertexArray& va, GLIndexBuffer& ib, GLShaderProgram& shaderP) const;
};

#endif // GLRENDERER_H
