#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "debug_utils.h"
#include "RAII_GLFWInitialization.h"
#include "RAII_GLFWwindow.h"

#include "GLRenderer.h"

#include "demos/DemoClearColor.h"

void error_callback(int error, const char* description) {
    std::cout << "GLFW-error [" << error << "]: " << description << '\n';
}

void update_window_size(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    // handle keys that are not meant for imgui here:
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(void)
{
    #ifdef NDEBUG
    std::cout << "RELEASE VERSION\n";
    #else
    std::cout << "DEBUG VERSION\n";
    #endif

    /* Initialize GLFW */
    glfwSetErrorCallback(error_callback);
    RAII_GLFWInitialization init; // constructor calls GLFWInit()
    if (!init.getSuccess()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // for anisotropic filtering without extension
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    RAII_GLFWwindow window(640, 480, "Hello World");
    if (!window.get())
    {
        //glfwTerminate(); called in destructor of GLFWInitialization
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window.get());

    // take care of viewport transform and aspect ratio:
    update_window_size(window.get(), 640, 480);
    glfwSetFramebufferSizeCallback(window.get(), update_window_size);

    // take care of keyboard handling:
    glfwSetKeyCallback(window.get(), key_callback);

    glfwSwapInterval(1);

    // initialize GLEW:
    if (glewInit() != GLEW_OK) {
        std::cout << "error: glewInit() failed!\n";
    }
    
    std::cout << glGetString(GL_VERSION) << '\n';

    // Setup Dear ImGui context:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();


    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    // -> install_callbacks = true -> callbacks will be installed
    //                              they will call the users previously installed callbacks (if any)
    ImGui_ImplOpenGL3_Init();

    GLRenderer renderer;

    demo::DemoClearColor myDemo(renderer);

    // bool show_demo_window = true;

    while (!glfwWindowShouldClose(window.get()))
    {
        // Poll for and process events:
        // (glfw-example does this at the end of the loop
        //  but imgui-example does it at the beginning.)
        // "... it is generally more correct and easier that you poll flags
        //  from the previous frame, then submit your inputs, then call NewFrame()."
        // https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-to-my-application
        glfwPollEvents();

        myDemo.OnUpdate(0.f);
        myDemo.OnRender();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        myDemo.OnImGuiRender();
        // if (show_demo_window) {
        //     ImGui::ShowDemoWindow(&show_demo_window);
        // }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.get());
    }

    // TODO: cherno destroys myDemo before ImGui-shutdown ?

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // The following are called after(!) the destructors of main()'s local variables:
    // glfwDestroyWindow(window.get()); in destructor of RAII_GLFWwindow
    // glfwTerminate(); in destructor of RAII_GLFWInitialization
    return 0;
}

