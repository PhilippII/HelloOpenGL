#ifndef RAII_GLFWINITIALIZATION_H
#define RAII_GLFWINITIALIZATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// this class is only responsible for calling
// glfInit() and glfwTerminate().
// The main point is that when this class is initialized
// before any other stack-allocated classes of the main function
// then glfwTerminate() will be called after the destructors
// of all of the other classes. (and not before)
class RAII_GLFWInitialization
{
public:
    RAII_GLFWInitialization() {
        m_success = (glfwInit() == GLFW_TRUE);
    }

    RAII_GLFWInitialization(const RAII_GLFWInitialization& other) = delete;
    RAII_GLFWInitialization& operator=(const RAII_GLFWInitialization& other) = delete;
    RAII_GLFWInitialization(RAII_GLFWInitialization&& other) = delete;
    RAII_GLFWInitialization& operator=(RAII_GLFWInitialization&& other) = delete;

    ~RAII_GLFWInitialization() {
        if (m_success) {
            glfwTerminate();
        }
    }

    bool getSuccess() const {
        return m_success;
    }
private:
    bool m_success;
};

#endif // RAII_GLFWINITIALIZATION_H
