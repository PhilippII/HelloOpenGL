#include "GLShaderProgram.h"

#include "debug_utils.h"

#include <iostream>

GLShaderProgram::GLShaderProgram(std::vector<ShaderSource> sources, SPReadiness readiness)
{
    m_rendererID = glCreateProgram();
    for (auto& src : sources) {
        addShaderFromSource(src);
    }
    bool success = makeReady(readiness);
    myAssert(success);
}

GLShaderProgram::GLShaderProgram(GLShaderProgram&& other)
    : m_rendererID(other.m_rendererID),
      m_shaders(std::move(other.m_shaders))
{
    other.m_rendererID = 0;
}

GLShaderProgram& GLShaderProgram::operator=(GLShaderProgram&& other) {
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;

    m_shaders = std::move(other.m_shaders);

    return *this;
}

GLShaderProgram::~GLShaderProgram() {
    if (m_rendererID) {
        GLint currId;
        GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currId));
        if (static_cast<GLuint>(currId) == m_rendererID) {
            GLCall(glUseProgram(0));
        }
        GLCall(glDeleteProgram(m_rendererID));
    }
    // docs.gl:
    // - shaders will be automatically detached
    // - programId zero (=moved from object) will be silently ignored
    //      (but in that case we do not need to call glUseProgram(0)
    //      so we check this case anyway.)
}

void GLShaderProgram::addShaderFromSource(const ShaderSource &source) {
    m_shaders.push_back(GLShader(source.type, source.sourceCode, false));
    GLCall(glAttachShader(m_rendererID, m_shaders.back().getName()));
}

bool GLShaderProgram::compileShaders() {
    bool success = true;
    for (auto& s : m_shaders) {
        success = (success && s.compile());
    }
    return success;
}

bool GLShaderProgram::link() {
    GLCall(glLinkProgram(m_rendererID));
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
    GLCall(glValidateProgram(m_rendererID));
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
    if (readiness == SPReadiness::BIND) {
        bind();
    }
    return buildSuccess;
}

bool GLShaderProgram::buildAll() {
    return makeReady(SPReadiness::VALIDATE);
}

void GLShaderProgram::bind() {
    GLCall(glUseProgram(m_rendererID));
}

void GLShaderProgram::unbind() {
#ifndef NDEBUG
    GLint currId;
    GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currId));
    myAssert(static_cast<GLuint>(currId) == m_rendererID);
#endif
    GLCall(glUseProgram(0));
}

void GLShaderProgram::printShaderProgramInfoLog() const {
    int length = getParam(GL_INFO_LOG_LENGTH);
    if (length > 0) {
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetProgramInfoLog(m_rendererID, length, nullptr, message));
        std::cout << message << '\n';
    } else {
        std::cout << "(no log)\n";
    }
}

GLint GLShaderProgram::getParam(GLenum pname) const {
    GLint param;
    myAssert(pname != GL_COMPUTE_WORK_GROUP_SIZE); // returns three integers
    GLCall(glGetProgramiv(m_rendererID, pname, &param));
    return param;
}


GLint GLShaderProgram::getAttribLocation(const std::string& name) const {
    GLCall(GLint location = glGetAttribLocation(m_rendererID, name.c_str()));
    return location;
}

GLint GLShaderProgram::getUniformLocation(const std::string &name) {
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        // cache hit:
        return uniformLocationCache[name];
    }
    // cache miss:
    GLCall(GLint location = glGetUniformLocation(m_rendererID, name.c_str()));
    if (location == -1) {
        std::cout << "warining: uniform " << name << " does not exist!\n";
    }
    uniformLocationCache[name] = location;
    return location;
    // docs.gl:
    // "This function returns -1 if name does not correspond
    //    to an active uniform variable in program, [...]"
}

void GLShaderProgram::setUniform4f(GLint location, float v0, float v1, float v2, float v3)
{
    myAssert(isBound());
    GLCall(glUniform4f(location, v0, v1, v2, v3));
    // docs.gl:
    // "If location is equal to -1, the data passed in will be silently ignored
    //    and the specified uniform variable will not be changed."
}

void GLShaderProgram::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    setUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

bool GLShaderProgram::isBound() const
{
    GLint programID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID);
    return (m_rendererID == programID);
}

