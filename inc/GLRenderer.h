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

    void draw(GLVertexArray& va, GLIndexBuffer& ib, GLShaderProgram& shaderP) const;
};

#endif // GLRENDERER_H
