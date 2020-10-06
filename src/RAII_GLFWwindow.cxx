#include "RAII_GLFWwindow.h"

RAII_GLFWwindow::RAII_GLFWwindow(int width, int height, const char *title)
    : window(glfwCreateWindow(width, height, title, NULL, NULL))
{}

RAII_GLFWwindow::RAII_GLFWwindow(RAII_GLFWwindow &&other) : window(other.window)
{
    other.window = NULL;
}

RAII_GLFWwindow &RAII_GLFWwindow::operator=(RAII_GLFWwindow &&other)
{
    if (this == &other) {
        return *this;
    }
    window = other.window;
    other.window = NULL;
    return *this;
}




RAII_GLFWwindow::~RAII_GLFWwindow()
{
   if (window)  {
       glfwDestroyWindow(window);
   }
}
