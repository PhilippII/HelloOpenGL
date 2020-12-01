#ifndef GLBUFFER_H_INCLUDED
#define GLBUFFER_H_INCLUDED

#include <GL/glew.h>

#include "debug_utils.h"

class GLBufferObject {
public:
    // glBufferData     (GLsizeiptr)
    // glNamedBufferData(GLsizei)
    using size_type = GLsizeiptr;

    GLBufferObject(GLenum target, size_type size, const GLvoid* data, GLenum usage, bool keepBound = true);

    GLBufferObject() = delete;

    // do not allow copy-constructor/assignment:
    GLBufferObject(const GLBufferObject& other) = delete;

    GLBufferObject& operator=(const GLBufferObject& other) = delete;

	// but do allow moves:
    GLBufferObject(GLBufferObject&& other);

    GLBufferObject& operator=(GLBufferObject&& other);

    virtual ~GLBufferObject();

	void bind() {
        GLCall(glBindBuffer(m_target, m_rendererId));
	}

	void unbind() {
        myAssert(isBound());
        GLCall(glBindBuffer(m_target, 0));
	}

    bool isBound() const;

    GLuint getRendererID() const {
        return m_rendererId;
    }

private:
    static GLenum getBindingEnum(GLenum target);

    GLenum m_target;
    GLuint m_rendererId;
};


#endif // GLBUFFER_H_INCLUDED

