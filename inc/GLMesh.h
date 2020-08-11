#ifndef GLMESH_H
#define GLMESH_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "debug_utils.h"

#include "GLBufferObject.h"
#include "GLVertexBuffer.h"
#include "GLVertexArrayObject.h"


#include "VertexBufferLayout.h"


/*
 * A class that creates and takes ownership of both:
 * - a VAO and
 * - an associated VBO referenced by the VAOs attributes
 *
 * Furthermore relative offsets and stride are computed automatically
 * from the order of the attributes and their types.
 *
 * Warning: you should unbind any previously bound VAOs before calling
 *          the constructor of this class.
 *
 */
class GLMesh
{
public:
    GLMesh() = delete;
    GLMesh(const VertexBufferLayout& layout, const GLvoid* vertexData, GLsizei vertexCount,
           GLenum indicesType, const GLvoid* indexData, GLsizei indexCount,
           bool keepBound = true);

    GLMesh(const GLMesh& other) = delete;
    GLMesh& operator=(const GLMesh& other) = delete;

    GLMesh(GLMesh&& other);
    GLMesh& operator=(GLMesh&& other);

    virtual ~GLMesh();

    void bindVAO() {
        m_vao.bind();
    }

    void unbindVAO() {
        m_vao.unbind();
    }

    GLsizei getNumUniqueVertices() const {
        return m_vertexCount;
    }

    GLsizei getNumIndices() const {
        return m_indexCount;
    }

    GLenum getIndicesType() const {
        return m_indicesType;
    }

private:
    static GLuint getIndexSize(GLenum indicesType);
    static bool isFloatingPoint(GLenum componentType);

    static const GLuint bindingIndex;

    GLVertexArrayObject m_vao;        // important that vao is initialized before
                                    // ibo is initialized, so vao is already bound
                                    // when ibo will be bound
    GLVertexBuffer m_vb;
    GLBufferObject m_ibo;
    GLsizei m_vertexCount;
    GLsizei m_indexCount;
    GLenum m_indicesType;
};

#endif // GLMESH_H
