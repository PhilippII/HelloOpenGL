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
    GLBufferObject(GLBufferObject&& other) noexcept;
    GLBufferObject& operator=(GLBufferObject&& other);
    // warning: moved from object (other) should be destroyed
    //          or assigned to before being used again
    // TODO: how to make move-assignment noexcept? (is glDeleteBuffers(..) noexcept ?)

    virtual ~GLBufferObject();

	void bind() {
        glBindBuffer(m_target, m_rendererId);
	}

	void unbind() {
        ASSERT(isBound());
        glBindBuffer(m_target, 0);
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

