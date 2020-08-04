#include "GLMesh.h"

const GLuint GLMesh::bindingIndex = 0;


GLMesh::GLMesh(GLsizei vertexCount,
               std::vector<VertexAttributeType> attrTypes, const GLvoid* vertexData,
               GLenum indicesType, const GLvoid* indexData)
    : offsets(computeOffsets(attrTypes)),
      vbo(GL_ARRAY_BUFFER, vertexCount * offsets.back(), vertexData, GL_STATIC_DRAW),
      ibo(GL_ELEMENT_ARRAY_BUFFER, vertexCount * getIndexSize(indicesType), indexData, GL_STATIC_DRAW),
      vao()
{
    vao.bind();
    GLCall(glBindVertexBuffer(bindingIndex, vbo.getName(), 0, offsets.back()));
    for (int i = 0; i < attrTypes.size(); ++i) {
        GLCall(glEnableVertexAttribArray(attrTypes[i].location));
        switch (attrTypes[i].castTo) {
          case VariableType::FLOAT:
            GLCall(glVertexAttribFormat(attrTypes[i].location, attrTypes[i].dimension,
                                        attrTypes[i].componentType, attrTypes[i].normalized,
                                        offsets[i]));
            break;
          case VariableType::DOUBLE:
            GLCall(glVertexAttribIFormat(attrTypes[i].location, attrTypes[i].dimension,
                                         attrTypes[i].componentType,
                                         offsets[i]));
            break;
          case VariableType::INT:
            GLCall(glVertexAttribLFormat(attrTypes[i].location, attrTypes[i].dimension,
                                         attrTypes[i].componentType,
                                         offsets[i]));
            break;
          default:
            myAssert(false);
            break;
        }
        GLCall(glVertexAttribBinding(attrTypes[i].location, bindingIndex));
    }
    ibo.bind();
    vao.unbind();
}

GLMesh::GLMesh(GLMesh&& other)
    : offsets(std::move(other.offsets)),
      vbo(std::move(other.vbo)), ibo(std::move(other.ibo)),
      vao(std::move(other.vao))
{}

GLMesh& GLMesh::operator=(GLMesh&& other) {
    offsets = std::move(other.offsets);
    vbo = std::move(other.vbo);
    ibo = std::move(other.ibo);
    vao = std::move(other.vao);

    return *this;
}

GLMesh::~GLMesh() {
    // destructors of members are called automatically
}

GLuint GLMesh::getSize(GLint dimension, GLenum componentType) {
    switch (componentType) {
      case GL_HALF_FLOAT:
        return dimension * sizeof(GLhalf);
      case GL_FLOAT:
        return dimension * sizeof(GLfloat);
      case GL_DOUBLE:
        return dimension * sizeof(GLdouble);
      case GL_FIXED:
        return dimension * sizeof(GLfixed);
      case GL_BYTE:
        return dimension * sizeof(GLbyte);
      case GL_UNSIGNED_BYTE:
        return dimension * sizeof(GLubyte);
      case GL_SHORT:
        return dimension * sizeof(GLshort);
      case GL_UNSIGNED_SHORT:
        return dimension * sizeof(GLushort);
      case GL_INT:
        return dimension * sizeof(GLint);
      case GL_UNSIGNED_INT:
        return dimension * sizeof(GLuint);
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        myAssert(dimension == 4 || dimension == GL_BGRA);
        return sizeof(GLuint);
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        myAssert(dimension == 3);
        return sizeof(GLuint);
      default:
        myAssert(false);
        return 0;
    }
}

GLuint GLMesh::getIndexSize(GLenum indicesType) {
    switch (indicesType) {
      case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
      case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
      case GL_UNSIGNED_INT:
        return sizeof(GLuint);
      default:
        myAssert(false);
        return 0;
    }
}

std::vector<GLuint> GLMesh::computeOffsets(std::vector<VertexAttributeType> attrTypes) {
    std::vector<GLuint> offsets;
    offsets.push_back(0);
    for (auto& attrT : attrTypes) {
        offsets.push_back(offsets.back() + getSize(attrT.dimension, attrT.componentType));
    }
    return offsets;
}
