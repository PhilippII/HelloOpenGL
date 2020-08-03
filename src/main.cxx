#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <array>
#include <vector>

#include <cmath> // M_PI and cos(...) sin(...)

// #include <cassert>
#include <signal.h> // for SIGTRAP in myAssert

#ifdef NDEBUG
#define myAssert(x)
#else
#define myAssert(x) if (!(x)) raise(SIGTRAP)
#endif

#ifdef NDEBUG
#define GLCall(x) x
#else
#define GLCall(x) GLClearError();\
    x;\
    myAssert(GLLogCall(#x, __FILE__, __LINE__))
#endif

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
    // or just:
    // while (glGetError());
}

static bool GLLogCall(const char* function, const char* file, int line) {
    bool success = true;
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << "): " << function
                  << " " << file << ":" << line << '\n';
        success = false;
    }
    return success;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // error handling:
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile "
                << ((type == GL_VERTEX_SHADER) ? "vertex" : "fragment")
                << " shader\n";
        std::cout << message << '\n';
        GLCall(glDeleteShader(id));
        return 0;
    }

    // if no error return shader id:
    return id;
}

/*
 * - compile vertexShader
 * - compile fragmentShader
 * - link them both together into a single shader program
 * - return a unique identifier to that shader program
 */
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

struct Vertex {
    std::array<float, 2> pos;
    std::array<float, 4> color;
};

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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    // initialize shader:
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    myAssert(location != -1);

    GLCall(GLint posAttrIndex = glGetAttribLocation(shader, "position"));
    GLCall(GLint colAttrIndex = glGetAttribLocation(shader, "vs_in_color"));


    // initialize rectangle:
    Vertex rectVertices[4] = {
        {{-.5f, -.5f}, {0.f, 0.f, 0.f, 0.f}}, // 0
        {{ .5f, -.5f}, {0.f, 0.f, 0.f, 0.f}}, // 1
        {{ .5f,  .5f}, {0.f, 0.f, 0.f, 0.f}}, // 2
        {{-.5f,  .5f}, {0.f, 0.f, 0.f, 0.f}}  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };


    unsigned int rectVAO;
    GLCall(glGenVertexArrays(1, &rectVAO));
    GLCall(glBindVertexArray(rectVAO));

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), rectVertices, GL_STATIC_DRAW));

    unsigned int bindingIndex = 0;
    GLCall(glBindVertexBuffer(bindingIndex, buffer, 0, sizeof(Vertex)));
    if (posAttrIndex >= 0) {
        GLCall(glEnableVertexAttribArray(posAttrIndex));
        GLCall(glVertexAttribFormat(posAttrIndex, 2, GL_FLOAT, GL_FALSE, 0));
        GLCall(glVertexAttribBinding(posAttrIndex, bindingIndex));
        // GLCall(glVertexAttribPointer(posAttrIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
    }

    if (colAttrIndex >= 0) {
        GLCall(glEnableVertexAttribArray(colAttrIndex));
        GLCall(glVertexAttribFormat(colAttrIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex::pos)));
        GLCall(glVertexAttribBinding(colAttrIndex, bindingIndex));
        // GLCall(glVertexAttribPointer(colAttrIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        //                              reinterpret_cast<void*>(sizeof(Vertex::pos))));
    }

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    float r = .0f;
    float increment = .05f;


    // initialize star:
    int n_spikes = 5;
    constexpr float pi_f = static_cast<float>(M_PI);
    float dPhi = pi_f / n_spikes; // 2*M_PI / (2*n_spikes)
    float phi_0 = .5f * pi_f - dPhi;
    std::vector<float> radii {.1, .25};
    std::array<std::array<float, 4>, 2> colors {std::array<float, 4>{0.f, 0.f, 0.f, 0.f},
                                                std::array<float, 4>{1.f, 1.f, 0.f, 1.f}};
    std::vector<Vertex> starVertices;
    for (int i = 0; i < 2 * n_spikes; ++i) {
        float phi_i = phi_0 + i * dPhi;
        float radius = radii[i % 2];
        starVertices.push_back(Vertex{
                                    std::array<float, 2>{radius * cosf(phi_i), radius * sinf(phi_i)},
                                    colors[i % 2]
                            });
    }
    starVertices.push_back(Vertex{
                                std::array<float, 2>{0.f, 0.f},
                                colors[0]
                            });

    std::vector<unsigned int> starIndices;
    for (int s = 0; s < n_spikes; ++s) {
        // inner triangle:
        starIndices.push_back(2 * s);
        starIndices.push_back((2 * s + 2) % (n_spikes * 2));
        starIndices.push_back(n_spikes * 2); // center point
        // outer triangle:
        starIndices.push_back(2 * s);
        starIndices.push_back(2 * s + 1);
        starIndices.push_back((2 * s + 2) % (n_spikes * 2));
    }

    unsigned int starVAO;
    GLCall(glGenVertexArrays(1, &starVAO));
    GLCall(glBindVertexArray(starVAO));

    unsigned int starVBO;
    GLCall(glGenBuffers(1, &starVBO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, starVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, starVertices.size() * sizeof(Vertex), starVertices.data(), GL_STATIC_DRAW));

    // unsigned int bindingIndex = 0;
    GLCall(glBindVertexBuffer(bindingIndex, starVBO, 0, sizeof(Vertex)));

    if (posAttrIndex >= 0) {
        GLCall(glEnableVertexAttribArray(posAttrIndex));
        GLCall(glVertexAttribFormat(posAttrIndex, 2, GL_FLOAT, GL_FALSE, 0));
        GLCall(glVertexAttribBinding(posAttrIndex, bindingIndex));
        // GLCall(glVertexAttribPointer(posAttrIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
    }

    if (colAttrIndex >= 0) {
        GLCall(glEnableVertexAttribArray(colAttrIndex));
        GLCall(glVertexAttribFormat(colAttrIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex::pos)));
        GLCall(glVertexAttribBinding(colAttrIndex, bindingIndex));
        // GLCall(glVertexAttribPointer(colAttrIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        //                             reinterpret_cast<void*>(sizeof(Vertex::pos))));
    }

    unsigned int starIBO;
    GLCall(glGenBuffers(1, &starIBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starIBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, starIndices.size() * sizeof(unsigned int), starIndices.data(), GL_STATIC_DRAW));



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // 1 draw rectangle:
        r += increment;
        if (r > 1.0) {
            r = 1.0;
            increment = -.05f;
        }
        if (r < 0.0) {
            r = 0.0;
            increment = +.05f;
        }

        // 1.1 bind shader (+ set uniforms):
        // TODO: bind shader
        GLCall(glUniform4f(location, r, .3f, .8f, 1.0f));

        // 1.2 bind vao:
        GLCall(glBindVertexArray(rectVAO));

        // 1.4 bind ibo:
        // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        // 1.5 draw:
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));


        // 2 draw star:
        // 2.1 bind shader (+ set uniforms):
        // TODO: bind shader
        GLCall(glUniform4f(location, .8f, .8f, .8f, 1.0f));

        // 2.2 bind vao:
        GLCall(glBindVertexArray(starVAO));

        // 2.4 bind ibo:
        // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starIBO));

        // 2.5 draw:
        GLCall(glDrawElements(GL_TRIANGLES, static_cast<int>(starIndices.size()),
                              GL_UNSIGNED_INT, nullptr));


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}

