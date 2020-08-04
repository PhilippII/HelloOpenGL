#include "GLShaderProgram.h"

#include "debug_utils.h"

#include <iostream>

GLShaderProgram::GLShaderProgram(std::vector<ShaderSource> sources, bool buildNow)
{
    programId = glCreateProgram();
    for (auto& src : sources) {
        addShaderFromSource(src);
    }
    if (buildNow) {
        bool success = buildAll();
        myAssert(success);
    }
}

GLShaderProgram::GLShaderProgram(GLShaderProgram&& other)
    : programId(other.programId),
      shaders(std::move(other.shaders))
{
    other.programId = 0;
}

GLShaderProgram& GLShaderProgram::operator=(GLShaderProgram&& other) {
    programId = other.programId;
    other.programId = 0;

    shaders = std::move(other.shaders);

    return *this;
}

GLShaderProgram::~GLShaderProgram() {
    if (programId) {
        GLint currId;
        GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currId));
        if (static_cast<GLuint>(currId) == programId) {
            GLCall(glUseProgram(0));
        }
        GLCall(glDeleteProgram(programId));
    }
    // docs.gl:
    // - shaders will be automatically detached
    // - programId zero (=moved from object) will be silently ignored
    //      (but in that case we do not need to call glUseProgram(0)
    //      so we check this case anyway.)
}

void GLShaderProgram::addShaderFromSource(const ShaderSource &source) {
    shaders.push_back(GLShader(source.type, source.sourceCode, false));
    GLCall(glAttachShader(programId, shaders.back().getName()));
}

bool GLShaderProgram::compileShaders() {
    bool success = true;
    for (auto& s : shaders) {
        success = (success && s.compile());
    }
    return success;
}

bool GLShaderProgram::link() {
    GLCall(glLinkProgram(programId));
    bool success = (getParam(GL_LINK_STATUS) == GL_TRUE);
    if (success) {
        std::cout << "shader program linked successfully. Log:\n";
    } else {
        std::cout << "error linking shader program! Log:\n";
    }
    printShaderProgramInfoLog();
    return success;
}

bool GLShaderProgram::validate() {
    GLCall(glValidateProgram(programId));
    bool success = (getParam(GL_VALIDATE_STATUS) == GL_TRUE);
    if (success) {
        std::cout << "shader program validated successfully. Log:\n";
    } else {
        std::cout << "error validating shader program! Log:\n";
    }
    printShaderProgramInfoLog();
    return success;
}

bool GLShaderProgram::buildAll() {
    return (compileShaders() && link()) && validate();
    // evaluation order left to right according to
    // https://stackoverflow.com/questions/7925479/if-argument-evaluation-order
    // also avoids evaluating right function if left one fails
}

void GLShaderProgram::use() {
    GLCall(glUseProgram(programId));
}

void GLShaderProgram::unuse() {
#ifndef NDEBUG
    GLint currId;
    GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currId));
    myAssert(static_cast<GLuint>(currId) == programId);
#endif
    GLCall(glUseProgram(0));
}

void GLShaderProgram::printShaderProgramInfoLog() const {
    int length = getParam(GL_INFO_LOG_LENGTH);
    if (length > 0) {
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetProgramInfoLog(programId, length, nullptr, message));
        std::cout << message << '\n';
    } else {
        std::cout << "(no log)\n";
    }
}

GLint GLShaderProgram::getParam(GLenum pname) const {
    GLint param;
    myAssert(pname != GL_COMPUTE_WORK_GROUP_SIZE); // returns three integers
    GLCall(glGetProgramiv(programId, pname, &param));
    return param;
}
