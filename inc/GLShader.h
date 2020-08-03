#ifndef GLSHADER_H
#define GLSHADER_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug_utils.h"

#include <string>

class GLShader
{
public:
    GLShader(GLenum type, const std::string& source, bool compileNow = true);

    // do not allow copying:
    GLShader(const GLShader& other) = delete;

    GLShader& operator=(const GLShader& other) = delete;

    // do allow move:
    GLShader(GLShader&& other);

    GLShader& operator=(GLShader&& other);

    ~GLShader();

    bool compile();

    void printInfoLog();

    GLuint getName() const {
        return shaderId;
    }

    GLint getParam(GLenum pname) const;

private:
    enum ShaderState {
        EMPTY, SOURCE, COMPILE_SUCCESS, COMPILE_ERROR
    };
    GLenum type;
    GLuint shaderId;
    ShaderState state = ShaderState::EMPTY;
};

#endif // GLSHADER_H
