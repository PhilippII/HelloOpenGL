#include "VertexBufferLayout.h"

#include "debug_utils.h"

VertexBufferLayout::VertexBufferLayout()
    : m_stride(0)
{

}

void VertexBufferLayout::append(GLint dimCount, GLenum componentType, VariableType castTo, GLint location)
{
    if (location == AUTO_LOCATION) {
        location = m_attributes.size();
    }
    m_attributes.push_back({static_cast<unsigned int>(m_stride),
                            dimCount, componentType, castTo,
                            static_cast<GLuint>(location)});
    m_stride += getAttributeSize(dimCount, componentType);
}

VertexBufferLayout &VertexBufferLayout::operator+=(const VertexBufferLayout &other)
{
   for (auto& attr : other.getAttributes()) {
       m_attributes.push_back(attr);
       m_attributes.back().offset += static_cast<unsigned int>(m_stride);
   }
   m_stride += other.getStride();
}

void VertexBufferLayout::setDefaultLocations()
{
    for (unsigned int i = 0; i < m_attributes.size(); ++i) {
        m_attributes[i].location = i;
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
