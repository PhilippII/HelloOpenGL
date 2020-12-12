#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <chrono>

#include "debug_utils.h"

#include "GLRenderer.h"

#include "demos/DemoClearColor.h"
#include "demos/DemoMultipleConcepts.h"
#include "demos/DemoLoadOBJ.h"
#include "demos/DemoPhongReflectionModel.h"
#include "demos/DemoPhongReflectionModelTextured.h"
#include "demos/DemoLinearColorspace.h"
#include "demos/DemoFilter.h"
#include "demos/DemoFramebuffer.h"


namespace raii_fy {
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

    class GLFW_Window
    {
    public:
        GLFW_Window() = delete;
        GLFW_Window(int width, int height, const char* title)
            : m_window(glfwCreateWindow(width, height, title, NULL, NULL))
        {}
        GLFW_Window(const GLFW_Window& other) = delete;
        GLFW_Window& operator=(const GLFW_Window& other) = delete;
        GLFW_Window(GLFW_Window&& other) = delete;
        GLFW_Window& operator=(GLFW_Window&& other) = delete;

        ~GLFW_Window() {
            if (m_window)  {
                glfwDestroyWindow(m_window);
            }
         }

        const ::GLFWwindow* get() const { return m_window; }
        ::GLFWwindow* get() { return m_window; }
    private:
        ::GLFWwindow* m_window;
    };

    class ImGuiContext {
    public:
        ImGuiContext() {
            ImGui::CreateContext();
        }
        ImGuiContext(const ImGuiContext& other) = delete;
        ImGuiContext(ImGuiContext&& other) = delete;
        ImGuiContext& operator=(const ImGuiContext& other) = delete;
        ImGuiContext& operator=(ImGuiContext&& other) = delete;

        ~ImGuiContext() {
            ImGui::DestroyContext();
        }
    };

    class ImGui_GLFW {
    public:
        ImGui_GLFW(::GLFWwindow* window, bool install_callbacks) {
            ImGui_ImplGlfw_InitForOpenGL(window, install_callbacks);
        }
        ImGui_GLFW(const ImGui_GLFW& other) = delete;
        ImGui_GLFW(ImGui_GLFW&& other) = delete;
        ImGui_GLFW& operator=(const ImGui_GLFW& other) = delete;
        ImGui_GLFW& operator=(ImGui_GLFW&& other) = delete;

        ~ImGui_GLFW() {
            ImGui_ImplGlfw_Shutdown();
        }
    };

    class ImGui_OpenGL3 {
    public:
        ImGui_OpenGL3(const char* glsl_version = NULL) {
            ImGui_ImplOpenGL3_Init(glsl_version);
        }
        ImGui_OpenGL3(const ImGui_OpenGL3& other) = delete;
        ImGui_OpenGL3(ImGui_OpenGL3&& other) = delete;
        ImGui_OpenGL3& operator=(const ImGui_OpenGL3& other) = delete;
        ImGui_OpenGL3& operator=(ImGui_OpenGL3&& other) = delete;

        ~ImGui_OpenGL3() {
            ImGui_ImplOpenGL3_Shutdown();
        }
    };
}


std::weak_ptr<demo::Demo> demo_global_ptr;

// OpenGL callback:
void GLAPIENTRY gl_debug_output_callback(GLenum source, GLenum type,
                                         GLuint id, GLenum severity,
                                         GLsizei /*length*/, const GLchar* message,
                                         const void* /*userParam*/) {
    std::ostream& os {(type == GL_DEBUG_SEVERITY_NOTIFICATION) ? std::cout : std::cerr};
    os << ((type == GL_DEBUG_TYPE_ERROR) ? "[GL-ERROR] " : "[gl-debug-message] ");
    os << message << '\n';
    os << std::hex << "(source = 0x" << source << " type = 0x" << type << " severity = 0x" << severity
       << " id = 0x" << id << ")\n" << std::dec;
    ASSERT(severity == GL_DEBUG_SEVERITY_NOTIFICATION);
}


// GLFW callbacks:
void error_callback(int error, const char* description) {
    std::cout << "GLFW-error [" << error << "]: " << description << '\n';
}

// uncomment unused parameter name to avoid -Wunused-parameter warning:
void update_window_size(GLFWwindow* /*window*/, int width, int height) {
    if (auto demo_sp = demo_global_ptr.lock()) {
        demo_sp->OnWindowSizeChanged(width, height);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) {
        // do nothing Dear ImGui handles it for us
    } else {
        auto demo_sp = demo_global_ptr.lock();
        if (demo_sp && demo_sp->OnKeyPressed(key, scancode, action, mods)) {
            // do nothing the demo handled it for us
        } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else {
            // ignore key
        }
    }
}


// main function:
int main(int argc, char **argv)
{
    #ifdef NDEBUG
    std::cout << "RELEASE VERSION\n";
    #else
    std::cout << "DEBUG VERSION\n";
    #endif

    /* Initialize GLFW */
    glfwSetErrorCallback(error_callback);
    raii_fy::GLFWInitialization init; // constructor calls GLFWInit()
    if (!init.getSuccess()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // glTexStorage2D(...) requires OpenGL 4.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // for anisotropic filtering without extension
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    raii_fy::GLFW_Window window(960, 640, "Hello World");
    if (!window.get())
    {
        //glfwTerminate(); called in destructor of raii_fy::GLFWInitialization
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window.get());

    // set up glfw callbacks:
    glfwSetFramebufferSizeCallback(window.get(), update_window_size);
    glfwSetKeyCallback(window.get(), key_callback);

    glfwSwapInterval(1);

    // initialize GLEW:
    if (glewInit() != GLEW_OK) {
        std::cout << "error: glewInit() failed!\n";
    }
    
    std::cout << glGetString(GL_VERSION) << '\n';

    // set up OpenGL Debug Output:
#ifndef NDEBUG
    // rememeber we did: glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    // so our OpenGL-context should be in Debug mode now
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output_callback, nullptr);
#endif

    // Setup Dear ImGui context:
    IMGUI_CHECKVERSION();
    raii_fy::ImGuiContext imgui_context;
    //ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    raii_fy::ImGui_GLFW imgui_glfw(window.get(), true);
    // -> install_callbacks = true -> callbacks will be installed
    //                              they will call the users previously installed callbacks (if any)
    raii_fy::ImGui_OpenGL3 imgui_opengl3;

    GLRenderer renderer;

    std::shared_ptr<demo::DemoSuite> myDemoP = std::make_shared<demo::DemoSuite>(renderer);
    // we use a shared pointer to share it with the glfw-callbacks, which can only access the global namespace
    myDemoP->RegisterDemo<demo::DemoClearColor>("Clear Color");
    myDemoP->RegisterDemo<demo::DemoMultipleConcepts>("a bunch of stuff");
    myDemoP->RegisterDemo<demo::DemoLoadOBJ>("Load WavefrontOBJ-file");
    myDemoP->RegisterDemo<demo::DemoPhongReflectionModel>("Phong Reflection Model");
    myDemoP->RegisterDemo<demo::DemoPhongReflectionModelTextured>("Phong Reflection Model with Texture");
    myDemoP->RegisterDemo<demo::DemoLinearColorspace>("Linear Colorspace");
    myDemoP->RegisterDemo<demo::DemoFilter>("Filter");
    myDemoP->RegisterDemo<demo::DemoFramebuffer>("Framebuffer");
    if (argc >= 2) {
        myDemoP->SelectDemo(argv[1]);
    }

    demo_global_ptr = myDemoP;

    // bool show_demo_window = true;

    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);
    myDemoP->OnWindowSizeChanged(width, height);

    auto t_old = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(window.get()))
    {
        // Poll for and process events:
        // (glfw-example does this at the end of the loop
        //  but imgui-example does it at the beginning.)
        // "... it is generally more correct and easier that you poll flags
        //  from the previous frame, then submit your inputs, then call NewFrame()."
        // https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-to-my-application
        glfwPollEvents();

        // compute delta time:
        namespace chr = std::chrono;
        auto t_new = chr::steady_clock::now();
        auto deltaTime = t_new - t_old;
        t_old = t_new;
        // convert to seconds:
        using secondsPerTick = std::ratio<1>;
        float deltaSeconds = chr::duration_cast<chr::duration<float, secondsPerTick>>(deltaTime).count();

        myDemoP->OnUpdate(deltaSeconds);
        myDemoP->OnRender();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        myDemoP->OnImGuiRender();
        // if (show_demo_window) {
        //     ImGui::ShowDemoWindow(&show_demo_window);
        // }

        bool sRGB = renderer.isEnabled_framebuffer_sRGB();
        if (sRGB) renderer.disable_framebuffer_sRGB();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (sRGB) renderer.enable_framebuffer_sRGB();

        glfwSwapBuffers(window.get());
    }

    // The following are called after(!) the destructor of myDemo:
    // ImGui_ImplOpenGL3_Shutdown(); // in destructor of raii_fy::ImGui_OpenGL3
    // ImGui_ImplGlfw_Shutdown(); // in destructor of raii_fy::ImGui_GLFW
    // ImGui::DestroyContext(); // in destructor of raii_fy::ImGuiContext

    // glfwDestroyWindow(window.get()); in destructor of raii_fy::GLFW_Window
    // glfwTerminate(); in destructor of raii_fy::GLFWInitialization
    return 0;
}

