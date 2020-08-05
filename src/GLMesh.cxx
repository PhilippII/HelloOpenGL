#include "GLMesh.h"

const GLuint GLMesh::bindingIndex = 0;


GLMesh::GLMesh(std::vector<VertexAttributeType> attrTypes, const GLvoid* vertexData, GLsizei vertexCount,
               GLenum indicesType, const GLvoid* indexData, GLsizei indexCount,
               bool keepBound)
    : offsets(computeOffsets(attrTypes)),
      vao(),
      vbo(GL_ARRAY_BUFFER, vertexCount * offsets.back(), vertexData, GL_STATIC_DRAW, false),
      ibo(GL_ELEMENT_ARRAY_BUFFER, indexCount * getIndexSize(indicesType), indexData, GL_STATIC_DRAW),
      vertexCount(vertexCount), indexCount(indexCount)
{
    GLCall(glBindVertexBuffer(bindingIndex, vbo.getName(), 0, offsets.back()));
    for (unsigned int i = 0; i < attrTypes.size(); ++i) {
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
    // ibo bound automatically in constructor already
    if (!keepBound) {
        vao.unbind();
    }
}

GLMesh::GLMesh(GLMesh&& other)
    : offsets(std::move(other.offsets)),
      vao(std::move(other.vao)),
      vbo(std::move(other.vbo)), ibo(std::move(other.ibo)),
      vertexCount(other.vertexCount), indexCount(other.indexCount)
{}

GLMesh& GLMesh::operator=(GLMesh&& other) {
    offsets = std::move(other.offsets);
    vao = std::move(other.vao);
    vbo = std::move(other.vbo);
    ibo = std::move(other.ibo);
    vertexCount = other.vertexCount;
    indexCount = other.indexCount;

    return *this;
}

GLMesh::~GLMesh() {
    // destructors of members are called automatically
}

GLuint GLMesh::getSize(GLint dimension, GLenum componentType) {
    std::size_t result = 0;
    switch (componentType) {
      case GL_HALF_FLOAT:
        result = dimension * sizeof(GLhalf);
        break;
      case GL_FLOAT:
        result = dimension * sizeof(GLfloat);
        break;
      case GL_DOUBLE:
        result = dimension * sizeof(GLdouble);
        break;
      case GL_FIXED:
        result = dimension * sizeof(GLfixed);
        break;
      case GL_BYTE:
        result = dimension * sizeof(GLbyte);
        break;
      case GL_UNSIGNED_BYTE:
        result = dimension * sizeof(GLubyte);
        break;
      case GL_SHORT:
        result = dimension * sizeof(GLshort);
        break;
      case GL_UNSIGNED_SHORT:
        result = dimension * sizeof(GLushort);
        break;
      case GL_INT:
        result = dimension * sizeof(GLint);
        break;
      case GL_UNSIGNED_INT:
        result = dimension * sizeof(GLuint);
        break;
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        myAssert(dimension == 4 || dimension == GL_BGRA);
        result = sizeof(GLuint);
        break;
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        myAssert(dimension == 3);
        result = sizeof(GLuint);
        break;
      default:
        myAssert(false);
        break;
    }
    return static_cast<GLuint>(result);
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
