#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filepath);
    Shader() = delete;
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;
    Shader(Shader&& other);
    Shader& operator=(Shader&& other);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    int GetUniformLocation(const std::string& name) const;
};

#endif // SHADER_H
