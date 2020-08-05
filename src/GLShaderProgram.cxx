#include "GLShaderProgram.h"

#include "debug_utils.h"

#include <iostream>

GLShaderProgram::GLShaderProgram(std::vector<ShaderSource> sources, SPReadiness readiness)
{
    programId = glCreateProgram();
    for (auto& src : sources) {
        addShaderFromSource(src);
    }
    bool success = makeReady(readiness);
    myAssert(success);
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

bool GLShaderProgram::makeReady(SPReadiness readiness) {
    // according to:
    // https://stackoverflow.com/questions/7925479/if-argument-evaluation-order
    // - evaluation order of && and || left to right according to
    // - also avoids evaluating right function if not necessary
    bool buildSuccess
         = ((readiness < SPReadiness::COMPILE)  || compileShaders())
        && ((readiness < SPReadiness::LINK)     || link())
        && ((readiness < SPReadiness::VALIDATE) || validate());
    if (readiness == SPReadiness::USE) {
        use();
    }
    return buildSuccess;
}

bool GLShaderProgram::buildAll() {
    return makeReady(SPReadiness::VALIDATE);
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


GLint GLShaderProgram::getAttribLocation(const std::string& name) {
    GLCall(GLint location = glGetAttribLocation(programId, name.c_str()));
    return location;
}

GLint GLShaderProgram::getUniformLocation(const std::string &name) {
    GLCall(GLint location = glGetUniformLocation(programId, name.c_str()));
    return location;
}

