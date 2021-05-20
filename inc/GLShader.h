#ifndef GLSHADER_H
#define GLSHADER_H


#include <GL/glew.h>

#include "debug_utils.h"

#include <string>

class GLShader
{
public:
    GLShader(GLenum type, const std::string& source, bool compileNow = true);

    GLShader() = delete;

    // do not allow copying:
    GLShader(const GLShader& other) = delete;

    GLShader& operator=(const GLShader& other) = delete;

    // do allow move:
    GLShader(GLShader&& other);

    GLShader& operator=(GLShader&& other);

    ~GLShader();

    bool compile();

    void printInfoLog();

    GLuint getRendererID() const {
        return m_rendererId;
    }

    GLint getParam(GLenum pname) const;

private:
    enum ShaderState {
        EMPTY, SOURCE, COMPILE_SUCCESS, COMPILE_ERROR
    };
    GLenum m_type;
    GLuint m_rendererId;
    ShaderState m_state = ShaderState::EMPTY;
};

#endif // GLSHADER_H
