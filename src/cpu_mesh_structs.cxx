#include "cpu_mesh_structs.h"

using std::cerr;

template <typename T>
static void printVec(std::ostream& os, GLint dimCount, T* p) {
    os << "{";
    if (dimCount > 0) {
        os << p[0];
    }
    for (int i = 1; i < dimCount; ++i) {
        os << ", " << p[i];
    }
    os << "}";
}

static void printAttribute(std::ostream& os, GLint dimCount, GLenum componentType, const GLvoid* data) {
    switch (componentType) {
      case GL_HALF_FLOAT:
        printVec(os, dimCount, reinterpret_cast<const GLhalf*>(data));
        break;
      case GL_FLOAT:
        printVec(os, dimCount, reinterpret_cast<const GLfloat*>(data));
        break;
      case GL_DOUBLE:
        printVec(os, dimCount, reinterpret_cast<const GLdouble*>(data));
        break;
      case GL_FIXED:
        printVec(os, dimCount, reinterpret_cast<const GLfixed*>(data));
        break;
      case GL_BYTE:
        printVec(os, dimCount, reinterpret_cast<const GLbyte*>(data));
        break;
      case GL_UNSIGNED_BYTE:
        printVec(os, dimCount, reinterpret_cast<const GLubyte*>(data));
        break;
      case GL_SHORT:
        printVec(os, dimCount, reinterpret_cast<const GLshort*>(data));
        break;
      case GL_UNSIGNED_SHORT:
        printVec(os, dimCount, reinterpret_cast<const GLushort*>(data));
        break;
      case GL_INT:
        printVec(os, dimCount, reinterpret_cast<const GLint*>(data));
        break;
      case GL_UNSIGNED_INT:
        printVec(os, dimCount, reinterpret_cast<const GLuint*>(data));
        break;
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        // myAssert(dimCount == 4 || dimCount == GL_BGRA);
        cerr << "printing of types GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV\n";
        cerr << "and GL_UNSIGNED_INT_10F_11F_11F_REV not supported\n";
        ASSERT(false);
        break;
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        // myAssert(dimCount == 3);
        cerr << "printing of types GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV\n";
        cerr << "and GL_UNSIGNED_INT_10F_11F_11F_REV not supported\n";
        ASSERT(false);
        break;
      default:
        ASSERT(false);
        break;
    }
}


std::ostream &operator<<(std::ostream &os, const CPUVertexArray &va)
{
    using va_size_t = std::vector<GLbyte>::size_type;
    using attrs_count_t = VertexBufferLayout::count_type;

    GLsizei stride = va.layout.getStride();
    va_size_t count = va.data.size() / stride;
    for (va_size_t i_v = 0; i_v < count; ++i_v) {
        os << "vertex " << i_v << ":\n{";
        const auto& attrs = va.layout.getAttributes();
        if (!attrs.empty()) {
            printAttribute(os, attrs[0].dimCount, attrs[0].componentType, &(va.data[i_v * stride + attrs[0].offset]));
        }
        for (attrs_count_t i_a = 1; i_a < attrs.size(); ++i_a) {
            os << ", ";
            printAttribute(os, attrs[i_a].dimCount, attrs[i_a].componentType, &(va.data[i_v * stride + attrs[i_a].offset]));
        }
        os << "}\n";
    }
    return os;
}
