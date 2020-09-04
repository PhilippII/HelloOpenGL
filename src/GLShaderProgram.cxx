#include "GLShaderProgram.h"

#include "debug_utils.h"

#include <iostream>

#include <fstream>
#include <sstream>
#include <regex>

using namespace std::literals::string_literals;


const std::unordered_map<std::string, GLenum> GLShaderProgram::shaderTypes ({
                {"compute"s, GL_COMPUTE_SHADER},
                {"vertex"s, GL_VERTEX_SHADER},
                {"tess_control"s, GL_TESS_CONTROL_SHADER},
                {"tess_evaluation"s, GL_TESS_EVALUATION_SHADER},
                {"geometry"s, GL_GEOMETRY_SHADER},
                {"fragment"s, GL_FRAGMENT_SHADER}
            });



GLShaderProgram::GLShaderProgram()
{
    m_rendererID = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const std::filesystem::path& filepath, SPReadiness readiness)
{
    m_rendererID = glCreateProgram();

    std::vector<ShaderSource> sources = parseShader(filepath);
    for (auto& src : sources) {
        addShaderFromSource(src);
    }
    bool success = makeReady(readiness);
    myAssert(success);
}

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
    if (this == &other) {
        return *this;
    }
    if (m_rendererID) {
        if (isBound()) {
            unbind();
        }
        GLCall(glDeleteProgram(m_rendererID));
    }
    m_rendererID = other.m_rendererID;
    other.m_rendererID = 0;

    m_shaders = std::move(other.m_shaders);

    return *this;
}

GLShaderProgram::~GLShaderProgram() {
    if (m_rendererID) {
        if (isBound()) {
            unbind();
        }
        GLCall(glDeleteProgram(m_rendererID));
    }
    // docs.gl:
    // - shaders will be automatically detached
    // - m_rendererID zero (=moved from object) will be silently ignored.
    //      * but I do not want to call members isBound() / unbind() on
    //          a "moved-from object"
    //          so we check this case anyway.
    //          (isBound() may return true if m_rendererID is zero
    //          and nothing is bound. The resulting unnecessary call
    //          to unbind() would not really cause a problem. But
    //          I would still find this weird.)
}

void GLShaderProgram::addShaderFromSource(const ShaderSource &source) {
    m_shaders.push_back(GLShader(source.type, source.sourceCode, false));
    GLCall(glAttachShader(m_rendererID, m_shaders.back().getRendererID()));
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
    if (buildSuccess && readiness == SPReadiness::BIND) {
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
    myAssert(isBound());
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
    GLint currID;
    GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currID));
    return (m_rendererID == static_cast<GLuint>(currID));
}

std::vector<ShaderSource> GLShaderProgram::parseShader(const std::filesystem::path& filepath)
{
    std::ifstream stream(filepath);
    if (!stream) {
        std::cerr << "error opening file " << filepath << '\n';
        myAssert(false);
        return std::vector<ShaderSource>();
    }

    std::regex pat {R"(#shader\s+(\w+)\s*)"};

    std::vector<ShaderSource> sources;

    GLenum type;
    bool validType = false;
    while (stream) {
        // 1. find next '#shader' directive or eof and
        //      store the shader source encountered before that
        std::string line;
        std::ostringstream oss;
        std::smatch matches;
        if (validType) {
            while (std::getline(stream, line) && !std::regex_match(line, matches, pat)) {
                oss << line << '\n';
            }
            sources.push_back(ShaderSource{type, oss.str()});
        } else {
            while (std::getline(stream, line) && !std::regex_match(line, matches, pat)) {}
        }

        // 2. figure out type for next shader to parse:
        if (stream) {
            std::string typeStr = matches[1].str();
            if (shaderTypes.find(typeStr) == shaderTypes.end()) {
                std::cerr << "warining: invalid shader type " << typeStr << '\n';
                myAssert(false);
                validType = false;
            } else {
                // TODO: difference between at(...) and operator[](...)?
                type = shaderTypes.at(typeStr);
                validType = true;
            }
        }
    }

    return sources;
}

