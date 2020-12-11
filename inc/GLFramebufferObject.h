#ifndef GLFRAMEBUFFEROBJECT_H
#define GLFRAMEBUFFEROBJECT_H

#include <GL/glew.h>

class GLFramebufferObject
{
public:
    GLFramebufferObject();
    // do not allow copy:
    GLFramebufferObject(const GLFramebufferObject& other) = delete;
    GLFramebufferObject& operator=(const GLFramebufferObject& other) = delete;
    // do allow move:
    GLFramebufferObject(GLFramebufferObject&& other);
    GLFramebufferObject& operator=(GLFramebufferObject&& other);

    ~GLFramebufferObject();

private:
    GLuint m_rendererId;
};

#endif // GLFRAMEBUFFEROBJECT_H
