#include "GLBufferObject.h"

GLBufferObject::GLBufferObject(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage, bool keepBound) {
	this->m_target = target;
    glGenBuffers(1, &(this->m_rendererId));
    glBindBuffer(target, this->m_rendererId);
    glBufferData(target, size, data, usage);
    if (!keepBound) {
        glBindBuffer(target, 0);
    }
}

GLBufferObject::GLBufferObject(GLBufferObject&& other) {
	m_target = other.m_target;
    m_rendererId = other.m_rendererId;
    other.m_rendererId = 0;
}

GLBufferObject& GLBufferObject::operator=(GLBufferObject&& other) {
    if (this == &other) {
        return *this;
    }
    if (m_rendererId) {
        glDeleteBuffers(1, &m_rendererId);
	}
	m_target = other.m_target;
    m_rendererId = other.m_rendererId;
    other.m_rendererId = 0;
	return *this;
}


GLBufferObject::~GLBufferObject() {
    if (m_rendererId) {
        glDeleteBuffers(1, &m_rendererId);
    }
}

bool GLBufferObject::isBound() const
{
    GLint currBuff;
    glGetIntegerv(getBindingEnum(m_target), &currBuff);
    return (static_cast<GLenum>(currBuff) == m_rendererId);
}

GLenum GLBufferObject::getBindingEnum(GLenum target) {
    switch (target) {
      case GL_ARRAY_BUFFER:
        return GL_ARRAY_BUFFER_BINDING;
      case GL_ATOMIC_COUNTER_BUFFER:
        return GL_ATOMIC_COUNTER_BUFFER_BINDING;
      case GL_COPY_READ_BUFFER:
        return GL_COPY_READ_BUFFER_BINDING;
      case GL_COPY_WRITE_BUFFER:
        return GL_COPY_WRITE_BUFFER_BINDING;
      case GL_DISPATCH_INDIRECT_BUFFER:
        return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
      case GL_DRAW_INDIRECT_BUFFER:
        return GL_DRAW_INDIRECT_BUFFER_BINDING;
      case GL_ELEMENT_ARRAY_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER_BINDING;
      case GL_PIXEL_PACK_BUFFER:
        return GL_PIXEL_PACK_BUFFER_BINDING;
      case GL_PIXEL_UNPACK_BUFFER:
        return GL_PIXEL_UNPACK_BUFFER_BINDING;
      case GL_QUERY_BUFFER:
        return GL_QUERY_BUFFER_BINDING;
      case GL_SHADER_STORAGE_BUFFER:
        return GL_SHADER_STORAGE_BUFFER_BINDING;
      case GL_TEXTURE_BUFFER:
        return GL_TEXTURE_BUFFER_BINDING;
      case GL_TRANSFORM_FEEDBACK_BUFFER:
        return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
      case GL_UNIFORM_BUFFER:
        return GL_UNIFORM_BUFFER_BINDING;
      default:
        myAssert(false);
        return 0;
    }
}
