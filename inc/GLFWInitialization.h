#ifndef GLFWIINITIALIZATION_H
#define GLFWIINITIALIZATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// this class is only responsible for calling
// glfInit() and glfwTerminate().
// The main point is that when this class is initialized
// before any other stack-allocated classes of the main function
// then glfwTerminate() will be called after the destructors
// of all of the other classes. (and not before)
class GLFWInitialization
{
public:
    GLFWInitialization() {
        m_success = (glfwInit() == GLFW_TRUE);
    }

    GLFWInitialization(const GLFWInitialization& other) = delete;
    GLFWInitialization& operator=(const GLFWInitialization& other) = delete;
    GLFWInitialization(GLFWInitialization&& other) = delete;
    GLFWInitialization& operator=(GLFWInitialization&& other) = delete;

    ~GLFWInitialization() {
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

#endif // GLFWIINITIALIZATION_H
