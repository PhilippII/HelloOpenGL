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

class GLShaderProgram
{
public:
    GLShaderProgram(std::vector<ShaderSource> sources, bool buildNow = true);

    GLShaderProgram(const GLShaderProgram& other) = delete;

    GLShaderProgram& operator=(const GLShaderProgram& other) = delete;

    GLShaderProgram(GLShaderProgram&& other);

    GLShaderProgram& operator=(GLShaderProgram&& other);

    virtual ~GLShaderProgram();

    void addShaderFromSource(const ShaderSource& source);

    bool compileShaders();


    bool link();

    bool validate();

    bool buildAll();

    void use();

    void unuse();

    GLint getParam(GLenum pname) const;

    GLuint getName() const {
        return programId;
    }

private:
    void printShaderProgramInfoLog() const;
    GLuint programId;
    std::vector<GLShader> shaders;
};

#endif // GLSHADERPROGRAM_H
