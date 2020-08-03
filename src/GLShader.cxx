#include "GLShader.h"

#include <iostream>

GLShader::GLShader(GLenum type, const std::string& source, bool compileNow)
    : type(type)
{
    GLCall(shaderId = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(shaderId, 1, &src, nullptr));
    state = GLShader::ShaderState::SOURCE;
    if (compileNow) {
        this->compile();
    }
}

GLShader::GLShader(GLShader&& other)
    : type(other.type), shaderId(other.shaderId), state(other.state)
{
    other.shaderId = 0;
}

GLShader& GLShader::operator=(GLShader&& other) {
    type = other.type;
    shaderId = other.shaderId;
    state = other.state;

    other.shaderId = 0;

    return *this;
}

GLShader::~GLShader() {
    if (shaderId) {
        GLCall(glDeleteShader(shaderId));
    }
}

bool GLShader::compile() {
    if (state == GLShader::ShaderState::SOURCE) {
        GLCall(glCompileShader(shaderId));

        bool success = (getParam(GL_COMPILE_STATUS) == GL_TRUE);
        if (success) {
            state = GLShader::ShaderState::COMPILE_SUCCESS;
            std::cout << "compiled successfully "
                    << ((type == GL_VERTEX_SHADER) ? "vertex" : "fragment")
                    << " shader\n";
        } else {
            state = GLShader::ShaderState::COMPILE_ERROR;
            std::cout << "Failed to compile "
                    << ((type == GL_VERTEX_SHADER) ? "vertex" : "fragment")
                    << " shader\n";
        }
        printInfoLog();
        return success;
    } else {
        return (state == GLShader::ShaderState::COMPILE_SUCCESS);
    }
}

void GLShader::printInfoLog() {
    int length;
    GLCall(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
    if (length > 0) {
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetShaderInfoLog(shaderId, length, &length, message));
        std::cout << message << '\n';
    } else {
        std::cout << "(no log)\n";
    }
}

GLint GLShader::getParam(GLenum pname) const {
    GLint result;
    GLCall(glGetShaderiv(shaderId, pname, &result));
    return result;
}
