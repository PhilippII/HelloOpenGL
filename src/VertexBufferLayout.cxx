#include "VertexBufferLayout.h"

#include <iostream>

VertexBufferLayout::VertexBufferLayout()
    : m_stride(0)
{

}

void VertexBufferLayout::append(GLint dimCount, GLenum componentType, std::string name)
{
    VariableType castTo;
    switch (getTypeCategory(componentType)) {
    case TypeCategory::FLOAT_SINGLE_PREC:
        castTo = VariableType::FLOAT;
        break;
    case TypeCategory::FLOAT_DOUBLE_PREC:
        castTo = VariableType::DOUBLE;
        break;
    case TypeCategory::INT_PACKED:
    case TypeCategory::INT_NOT_PACKED:
        castTo = VariableType::NORMALIZED_FLOAT;
        break;
    default:
        myAssert(false);
        castTo = VariableType::FLOAT;
        break;
    }
    append(dimCount, componentType, castTo, name);
}

void VertexBufferLayout::append(GLint dimCount, GLenum componentType, VariableType castTo, loc_type location, std::string name)
{
    m_attributes.push_back({static_cast<unsigned int>(m_stride),
                            dimCount, componentType, castTo,
                            location, name});
    m_stride += getAttributeSize(dimCount, componentType);
}

VertexBufferLayout &VertexBufferLayout::operator+=(const VertexBufferLayout &other)
{
   for (auto& attr : other.getAttributes()) {
       m_attributes.push_back(attr);
       m_attributes.back().offset += static_cast<offset_type>(m_stride);
   }
   m_stride += other.getStride();
   return *this;
}

void VertexBufferLayout::setDefaultLocations()
{
    for (unsigned int i = 0; i < m_attributes.size(); ++i) {
        m_attributes[i].location = i;
    }
}

void VertexBufferLayout::setLocations(const GLShaderProgram &program)
{
    for (auto& attr: m_attributes) {
        if (!attr.name.empty()) {
            auto loc = program.getAttribLocation(attr.name);
            if (loc == -1) {
                std::cout << "warning: did not find attribute location " << attr.name << '\n';
            } else {
                attr.location = loc;
            }
        }
    }
}

VertexBufferLayout::TypeCategory VertexBufferLayout::getTypeCategory(GLenum componentType)
{
    switch (componentType) {
    case GL_HALF_FLOAT:
    case GL_FLOAT:
    case GL_FIXED:
        return VertexBufferLayout::TypeCategory::FLOAT_SINGLE_PREC;
    case GL_DOUBLE:
        return VertexBufferLayout::TypeCategory::FLOAT_DOUBLE_PREC;
    case GL_BYTE:
    case GL_SHORT:
    case GL_INT:
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_INT:
        return VertexBufferLayout::TypeCategory::INT_NOT_PACKED;
    default:
        return VertexBufferLayout::TypeCategory::INT_PACKED;
    }
}


GLuint VertexBufferLayout::getAttributeSize(GLint dimCount, GLenum componentType) {
    std::size_t result = 0;
    switch (componentType) {
      case GL_HALF_FLOAT:
        result = dimCount * sizeof(GLhalf);
        break;
      case GL_FLOAT:
        result = dimCount * sizeof(GLfloat);
        break;
      case GL_DOUBLE:
        result = dimCount * sizeof(GLdouble);
        break;
      case GL_FIXED:
        result = dimCount * sizeof(GLfixed);
        break;
      case GL_BYTE:
        result = dimCount * sizeof(GLbyte);
        break;
      case GL_UNSIGNED_BYTE:
        result = dimCount * sizeof(GLubyte);
        break;
      case GL_SHORT:
        result = dimCount * sizeof(GLshort);
        break;
      case GL_UNSIGNED_SHORT:
        result = dimCount * sizeof(GLushort);
        break;
      case GL_INT:
        result = dimCount * sizeof(GLint);
        break;
      case GL_UNSIGNED_INT:
        result = dimCount * sizeof(GLuint);
        break;
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        myAssert(dimCount == 4 || dimCount == GL_BGRA);
        result = sizeof(GLuint);
        break;
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        myAssert(dimCount == 3);
        result = sizeof(GLuint);
        break;
      default:
        myAssert(false);
        break;
    }
    return static_cast<GLuint>(result);
}

VertexBufferLayout operator+(VertexBufferLayout a, const VertexBufferLayout &b)
{
   return a += b;
}
