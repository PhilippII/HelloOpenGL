#include "Renderer.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
    // or just:
    // while (glGetError());
}

bool GLLogCall(const char* function, const char* file, int line) {
    bool success = true;
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << "): " << function
                  << " " << file << ":" << line << '\n';
        success = false;
    }
    return success;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
