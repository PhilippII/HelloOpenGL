#ifndef GLBUFFER_H_INCLUDED
#define GLBUFFER_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug_utils.h"

class GLBufferObject {
public:
    GLBufferObject(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage, bool keepBound = true);

	// do not allow copy-constructor/assignment
    GLBufferObject(const GLBufferObject& other) = delete;

    GLBufferObject& operator=(const GLBufferObject& other) = delete;

	// but do allow moves:
    GLBufferObject(GLBufferObject&& other);

    GLBufferObject& operator=(GLBufferObject&& other);

    ~GLBufferObject();

	void bind() {
        GLCall(glBindBuffer(target, buffer));
	}

	void unbind() {
		#ifndef NDEBUG
        GLint currBuff;
        GLCall(glGetIntegerv(getBindingEnum(target), &currBuff));
        myAssert(static_cast<GLenum>(currBuff) == buffer);
		#endif
        GLCall(glBindBuffer(target, 0));
	}

    GLuint getName() const {
        return buffer;
    }

private:
    static GLenum getBindingEnum(GLenum target);

	GLenum target;
	GLuint buffer;
};


#endif // GLBUFFER_H_INCLUDED

