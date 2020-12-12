#include "GLIndexBuffer.h"

GLuint GLIndexBuffer::getIndexSize(GLenum type) {
    switch (type) {
      case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
      case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
      case GL_UNSIGNED_INT:
        return sizeof(GLuint);
      default:
        ASSERT(false);
        return 0;
    }
}
