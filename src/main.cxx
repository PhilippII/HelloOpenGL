#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include "Shader.h"

#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(void)
{
    #ifdef NDEBUG
    std::cout << "RELEASE VERSION\n";
    #else
    std::cout << "DEBUG VERSION\n";
    #endif
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "error: glewInit() failed!\n";
    }
    
    std::cout << glGetString(GL_VERSION) << '\n';

    {
        float positions[4 * (2+2)] = {
            100.f, 100.f, 0.0f, 0.0f, // 0
            200.f, 100.f, 1.0f, 0.0f, // 1
            200.f, 200.f, 1.0f, 1.0f, // 2
            100.f, 200.f, 0.0f, 1.0f  // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexArray va;
        VertexBuffer vb(positions, 4 * (2+2) * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); // position
        layout.Push<float>(2); // texture coordinate
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(0.f, 960.f, 0.f, 540.f, -1.f, 1.f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", .8f, .3f, .8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", mvp);

        Texture texture("res/textures/face_transparent_test.png");
        unsigned int texSlot = 0;
        texture.Bind(texSlot);
        shader.SetUniform1i("u_Texture", texSlot);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;
        GLCall(glClearColor(.5f, .5f, .5f, 1.f)); // TODO: maybe put this in Renderer class
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        float r = .0f;
        float increment = .05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, .3f, .8f, 1.0f);

            renderer.Draw(va, ib, shader);

            r += increment;
            if (r > 1.0) {
                r = 1.0;
                increment = -.05f;
            }
            if (r < 0.0) {
                r = 0.0;
                increment = +.05f;
            }

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}

