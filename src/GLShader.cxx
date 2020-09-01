#include "GLShader.h"

#include <iostream>

GLShader::GLShader(GLenum type, const std::string& source, bool compileNow)
    : m_type(type)
{
    GLCall(m_rendererId = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(m_rendererId, 1, &src, nullptr));
    m_state = GLShader::ShaderState::SOURCE;
    if (compileNow) {
        this->compile();
    }
}

GLShader::GLShader(GLShader&& other)
    : m_type(other.m_type), m_rendererId(other.m_rendererId), m_state(other.m_state)
{
    other.m_rendererId = 0;
}

GLShader& GLShader::operator=(GLShader&& other) {
    if (this == &other) {
        return *this;
    }
    if (m_rendererId) {
        GLCall(glDeleteShader(m_rendererId));
    }
    m_type = other.m_type;
    m_rendererId = other.m_rendererId;
    m_state = other.m_state;

    other.m_rendererId = 0;

    return *this;
}

GLShader::~GLShader() {
    if (m_rendererId) {
        GLCall(glDeleteShader(m_rendererId));
    }
}

bool GLShader::compile() {
    if (m_state == GLShader::ShaderState::SOURCE) {
        GLCall(glCompileShader(m_rendererId));

        bool success = (getParam(GL_COMPILE_STATUS) == GL_TRUE);
        if (success) {
            m_state = GLShader::ShaderState::COMPILE_SUCCESS;
            std::cout << "compiled successfully "
                    << ((m_type == GL_VERTEX_SHADER) ? "vertex" : "fragment")
                    << " shader\n";
        } else {
            m_state = GLShader::ShaderState::COMPILE_ERROR;
            std::cout << "Failed to compile "
                    << ((m_type == GL_VERTEX_SHADER) ? "vertex" : "fragment")
                    << " shader\n";
        }
        printInfoLog();
        return success;
    } else {
        return (m_state == GLShader::ShaderState::COMPILE_SUCCESS);
    }
}

void GLShader::printInfoLog() {
    int length;
    GLCall(glGetShaderiv(m_rendererId, GL_INFO_LOG_LENGTH, &length));
    if (length > 0) {
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetShaderInfoLog(m_rendererId, length, &length, message));
        std::cout << message << '\n';
    } else {
        std::cout << "(no log)\n";
    }
}

GLint GLShader::getParam(GLenum pname) const {
    GLint result;
    GLCall(glGetShaderiv(m_rendererId, pname, &result));
    return result;
}
