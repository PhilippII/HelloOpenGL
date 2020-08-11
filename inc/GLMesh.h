#ifndef GLMESH_H
#define GLMESH_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "debug_utils.h"

#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLVertexArray.h"


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

    void bindVA() {
        m_va.bind();
    }

    void unbindVA() {
        m_va.unbind();
    }

    void bindIB() {
        m_ib.bind();
    }

    void unbindIB() {
        m_ib.unbind();
    }

    GLsizei getNumUniqueVertices() const {
        return m_vertexCount;
    }

    GLsizei getNumIndices() const {
        return m_ib.getCount();
    }

    GLenum getIndicesType() const {
        return m_ib.getType();
    }

private:
    static bool isFloatingPoint(GLenum componentType);

    static const GLuint bindingIndex;

    GLVertexBuffer m_vb;
    GLVertexArray m_va;
    GLIndexBuffer m_ib;     // m_ib should be constructed after m_va so it is bound
                            // in its constructor after m_va is bound its constructor.
                            // thus m_ib's binding will be stored in m_va.
    GLsizei m_vertexCount;
};

#endif // GLMESH_H
