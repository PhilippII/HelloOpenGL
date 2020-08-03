#include "GLBufferObject.h"

GLBufferObject::GLBufferObject(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
	this->target = target;
    GLCall(glGenBuffers(1, &(this->buffer)));
    GLCall(glBindBuffer(target, this->buffer));
    GLCall(glBufferData(target, size, data, usage));
    GLCall(glBindBuffer(target, 0));
}

GLBufferObject::GLBufferObject(GLBufferObject&& other) {
	target = other.target;
	buffer = other.buffer;
	other.buffer = 0;
}

GLBufferObject& GLBufferObject::operator=(GLBufferObject&& other) {
	if (buffer) {
        GLCall(glDeleteBuffers(1, &buffer));
	}
	target = other.target;
	buffer = other.buffer;
	other.buffer = 0;
	return *this;
}


GLBufferObject::~GLBufferObject() {
	if (buffer) {
        GLCall(glDeleteBuffers(1, &buffer));
	}
}

