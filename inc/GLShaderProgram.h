#ifndef GLSHADERPROGRAM_H
#define GLSHADERPROGRAM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLShader.h"

#include <filesystem> // for std::filesystem::path

#include <vector>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderSource {
    GLenum type;
    std::string sourceCode;
};

enum class SPReadiness {
    NONE=0, COMPILE=1, LINK=2, VALIDATE=3, BIND=4
};

class GLShaderProgram
{
public:
    GLShaderProgram();
    GLShaderProgram(const std::filesystem::path& filepath, SPReadiness readiness = SPReadiness::BIND);
    GLShaderProgram(std::vector<ShaderSource> sources, SPReadiness readiness = SPReadiness::BIND);

    GLShaderProgram(const GLShaderProgram& other) = delete;

    GLShaderProgram& operator=(const GLShaderProgram& other) = delete;

    GLShaderProgram(GLShaderProgram&& other);

    GLShaderProgram& operator=(GLShaderProgram&& other);

    virtual ~GLShaderProgram();

    void addShaderFromSource(const ShaderSource& source);

    bool compileShaders();


    bool link();

    bool validate();

    /*
     * this function will run through steps again
     * even if they were already finished before calling this
     * function.
     * E.g. if shaders were compiled already before calling this
     * function with SPReadiness::LINK they will be compiled a second time
     * before being linked
     */
    bool makeReady(SPReadiness readiness);

    bool buildAll();

    void bind();

    void unbind();

    GLint getParam(GLenum pname) const;

    GLuint getRendererID() const {
        return m_rendererID;
    }

    GLint getAttribLocation(const std::string& name) const;

    GLint getUniformLocation(const std::string& name) const;

    void setUniform1i(GLint location, int v);

    void setUniform1i(const std::string& name, int v);

    void setUniform1f(GLint location, float value);

    void setUniform1f(const std::string& name, float value);

    void setUniform3f(GLint location, float v0, float v1, float v2);

    void setUniform3f(const std::string& name, float v0, float v1, float v2);

    void setUniform3f(GLint location, const glm::vec3& vector);

    void setUniform3f(const std::string& name, const glm::vec3& vector);

    void setUniform4f(GLint location, float v0, float v1, float v2, float v3);

    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

    void setUniform4fv(GLint location, const GLfloat *value);

    void setUniform4fv(const std::string& name, const GLfloat* value);

    void setUniformMat4f(GLint location, const glm::mat4& matrix);

    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

    bool isBound() const;

private:
    static std::vector<ShaderSource> parseShader(const std::filesystem::path& filepath);
    static const std::unordered_map<std::string, GLenum> shaderTypes;

    void printShaderProgramInfoLog() const;
    GLuint m_rendererID;
    std::vector<GLShader> m_shaders;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};

#endif // GLSHADERPROGRAM_H
