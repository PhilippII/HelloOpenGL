#ifndef RAII_GLFWWINDOW_H
#define RAII_GLFWWINDOW_H

#include <GLFW/glfw3.h>

class RAII_GLFWwindow
{
public:
    RAII_GLFWwindow() = delete;
    RAII_GLFWwindow(int width, int height, const char* title);
    RAII_GLFWwindow(const RAII_GLFWwindow& other) = delete;
    RAII_GLFWwindow& operator=(const RAII_GLFWwindow& other) = delete;
    RAII_GLFWwindow(RAII_GLFWwindow&& other);
    RAII_GLFWwindow& operator=(RAII_GLFWwindow&& other);

    ~RAII_GLFWwindow();

    const GLFWwindow* get() const { return window; }
    GLFWwindow* get() { return window; }
private:
    GLFWwindow* window;
};

#endif // RAII_GLFWWINDOW_H
