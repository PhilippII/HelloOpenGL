#ifndef GLMESH_H
#define GLMESH_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "debug_utils.h"

#include "GLBufferObject.h"
#include "GLVertexArrayObject.h"

enum class VariableType {
    // type of the actual shader variable
    // which may be different from the type of the data in the buffer
    // (e.g. we might cast integers in the buffer to float variables in the shader
    FLOAT, DOUBLE, INT
};

struct VertexAttributeType {
    GLuint location;

    GLint dimension;
    GLenum componentType;
    GLboolean normalized;
    VariableType castTo;
};



/*
 * A class that creates and takes ownership of both:
 * - a VAO and
 * - an associated VBO referenced by the VAOs attributes
 *
 * Furthermore relative offsets and stride are computed automatically
 * from the order of the attributes and their types.
 *
 */
class GLMesh
{
public:
    GLMesh() = delete;
    GLMesh(GLsizei vertexCount, std::vector<VertexAttributeType> attrTypes, const GLvoid* vertexData, GLenum indicesType, const GLvoid* indexData);

    GLMesh(const GLMesh& other) = delete;
    GLMesh& operator=(const GLMesh& other) = delete;

    GLMesh(GLMesh&& other);
    GLMesh& operator=(GLMesh&& other);

    virtual ~GLMesh();

    void bindVAO() {
        vao.bind();
    }

    void unbindVAO() {
        vao.unbind();
    }

private:
    static GLuint getSize(GLint dimension, GLenum componentType);
    static GLuint getIndexSize(GLenum indicesType);
    static std::vector<GLuint> computeOffsets(std::vector<VertexAttributeType> attrTypes);

    std::vector<GLuint> offsets;    // important that this member is the first !
                                    // so it is also initialized first
    GLBufferObject vbo;
    GLBufferObject ibo;
    GLVertexArrayObject vao;
    static const GLuint bindingIndex;
};

#endif // GLMESH_H
