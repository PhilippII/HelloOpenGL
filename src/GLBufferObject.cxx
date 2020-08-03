#include "GLBufferObject.h"

GLBufferObject::GLBufferObject(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
	this->target = target;
	glGenBuffers(1, &(this->buffer));
	glBindBuffer(target, this->buffer);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
}

GLBufferObject::GLBufferObject(GLBufferObject&& other) {
	target = other.target;
	buffer = other.buffer;
	other.buffer = 0;
}

GLBufferObject& GLBufferObject::operator=(GLBufferObject&& other) {
	if (buffer) {
		glDeleteBuffers(1, &buffer);
	}
	target = other.target;
	buffer = other.buffer;
	other.buffer = 0;
	return *this;
}


GLBufferObject::~GLBufferObject() {
	if (buffer) {
		glDeleteBuffers(1, &buffer);
	}
}

