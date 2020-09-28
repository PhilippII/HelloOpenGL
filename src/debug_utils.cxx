#include "debug_utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
    // or just:
    // while (glGetError());
}

bool GLLogCall(const char* function, const char* file, int line) {
    bool success = true;
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (0x" << std::hex << error << std::dec << "): " << function
                  << " " << file << ":" << line << '\n';
        success = false;
    }
    return success;
}
