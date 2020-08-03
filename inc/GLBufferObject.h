#ifndef GLBUFFER_H_INCLUDED
#define GLBUFFER_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include "debug_utils.h"

class GLBufferObject {
public:
    GLBufferObject(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

	// do not allow copy-constructor/assignment
    GLBufferObject(const GLBufferObject& other) = delete;

    GLBufferObject& operator=(const GLBufferObject& other) = delete;

	// but do allow moves:
    GLBufferObject(GLBufferObject&& other);

    GLBufferObject& operator=(GLBufferObject&& other);

    ~GLBufferObject();

	void bind() {
        GLCall(glBindBuffer(target, buffer));
		#ifndef NDEBUG
		bound = true;
		#endif
	}

	void unbind() {
		#ifndef NDEBUG
		assert(bound);
		bound = false;
		#endif
        GLCall(glBindBuffer(target, 0));
	}

    GLuint getName() const {
        return buffer;
    }

private:
	GLenum target;
	GLuint buffer;
#ifndef NDEBUG
	bool bound = false;
#endif
};


#endif // GLBUFFER_H_INCLUDED

