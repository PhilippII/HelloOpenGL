#ifndef GLSHADERPROGRAM_H
#define GLSHADERPROGRAM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLShader.h"

#include <vector>
#include <string>

struct ShaderSource {
    GLenum type;
    std::string sourceCode;
};

enum class SPReadiness {
    NONE=0, COMPILE=1, LINK=2, VALIDATE=3, USE=4
};

class GLShaderProgram
{
public:
    GLShaderProgram(std::vector<ShaderSource> sources, SPReadiness readiness = SPReadiness::USE);

    GLShaderProgram(const GLShaderProgram& other) = delete;

    GLShaderProgram& operator=(const GLShaderProgram& other) = delete;

    GLShaderProgram(GLShaderProgram&& other);

    GLShaderProgram& operator=(GLShaderProgram&& other);

    virtual ~GLShaderProgram();

    void addShaderFromSource(const ShaderSource& source);

    bool compileShaders();


    bool link();

    bool validate();

    bool makeReady(SPReadiness readiness);

    bool buildAll();

    void use();

    void unuse();

    GLint getParam(GLenum pname) const;

    GLuint getName() const {
        return m_rendererID;
    }

    GLint getAttribLocation(const std::string& name);

    GLint getUniformLocation(const std::string& name);

private:
    void printShaderProgramInfoLog() const;
    GLuint m_rendererID;
    std::vector<GLShader> m_shaders;
};

#endif // GLSHADERPROGRAM_H
