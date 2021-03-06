#include "GLShader.h"

#include <iostream>
#include <utility> // std::move(..), std::exchange(..)

GLShader::GLShader(GLenum type, const std::string& source, bool compileNow)
    : m_type(type)
{
    m_rendererId = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(m_rendererId, 1, &src, nullptr);
    m_state = GLShader::ShaderState::SOURCE;
    if (compileNow) {
        this->compile();
    }
}

GLShader::GLShader(GLShader&& other) noexcept
    : m_type(std::move(other.m_type)),
      m_rendererId(std::exchange(other.m_rendererId, 0)),
      m_state(std::move(other.m_state))
{}

GLShader& GLShader::operator=(GLShader&& other) {
    if (this == &other) {
        return *this;
    }

    if (m_rendererId) {
        glDeleteShader(m_rendererId);
    }

    m_type = std::move(other.m_type);
    m_rendererId = std::exchange(other.m_rendererId, 0);
    m_state = std::move(other.m_state);

    return *this;
}

GLShader::~GLShader() {
    if (m_rendererId) {
        glDeleteShader(m_rendererId);
    }
}

bool GLShader::compile() {
    if (m_state == GLShader::ShaderState::SOURCE) {
        glCompileShader(m_rendererId);

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
    glGetShaderiv(m_rendererId, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(m_rendererId, length, &length, message);
        std::cout << message << '\n';
    } else {
        std::cout << "(no log)\n";
    }
}

GLint GLShader::getParam(GLenum pname) const {
    GLint result;
    glGetShaderiv(m_rendererId, pname, &result);
    return result;
}
