#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <array>
#include <vector>

#include <cmath> // M_PI and cos(...) sin(...)
#include <chrono>

// #include <cassert>


#include "debug_utils.h"
#include "GLFWInitialization.h"

#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"

#include "VertexBufferLayout.h"
#include "GLVertexArray.h"

#include "GLShader.h"
#include "GLShaderProgram.h"

#include "GLTexture.h"

#include "GLRenderer.h"

#include "cpu_mesh_import.h"
#include "cpu_mesh_utils.h" // to test addIndexBuffer(...)

namespace fs = std::filesystem;

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
    GLFWInitialization init; // constructor calls GLFWInit()
    if (!init.getSuccess()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        //glfwTerminate(); called in destructor of GLFWInitialization
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
    GLShaderProgram shaderProgram(fs::path("res/shaders/Basic.shader", fs::path::format::generic_format));
    // shaderProgram.bind() is called automatically in constructor

    //int location = shaderProgram.getUniformLocation("u_Color");
    //myAssert(location != -1);

    GLint posAttrIndex = shaderProgram.getAttribLocation("position");
    myAssert(posAttrIndex != -1);
    GLint colAttrIndex = shaderProgram.getAttribLocation("vs_in_color");
    myAssert(colAttrIndex != -1);


    // initialize house:
    float hSz = 1.5f;
    Vertex houseVertices[9] = {
        {{-.5f * hSz, -.5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{ .5f * hSz, -.5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{ .5f * hSz,  .5f * hSz}, {.5f, .5f, .5f, .0f}},

        {{-.5f * hSz, -.5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{ .5f * hSz,  .5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{-.5f * hSz,  .5f * hSz}, {.5f, .5f, .5f, .0f}},

        {{-.5f * hSz,  .5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{ .5f * hSz,  .5f * hSz}, {.5f, .5f, .5f, .0f}},
        {{ .0f * hSz,  .75f * hSz}, {.5f, .5f, .5f, .0f}}
    };
    VertexBufferLayout houseLayout;
    houseLayout.append<decltype(Vertex::pos)>(1, VariableType::FLOAT, posAttrIndex);
    houseLayout.append<decltype(Vertex::color)>(1, VariableType::FLOAT, colAttrIndex);
    CPUMesh<GLuint> houseCPUMesh = addIndexBuffer<GLuint>(CPUVertexArray{houseLayout,
                                                                 std::vector<GLbyte>(reinterpret_cast<const GLbyte*>(houseVertices),
                                                                                     reinterpret_cast<const GLbyte*>(houseVertices)
                                                                                                        + 9 * sizeof(Vertex))});
    std::cout << "houseCPUMesh:\n";
    std::cout << houseCPUMesh;
    GLVertexBuffer houseVB(houseCPUMesh.va.data.size(), houseCPUMesh.va.data.data());
    GLVertexArray houseVA;
    houseVA.addBuffer(houseVB, houseLayout);
    GLIndexBuffer houseIB(GL_UNSIGNED_INT,
                          static_cast<GLIndexBuffer::count_type>(houseCPUMesh.ib.indices.size()),
                          houseCPUMesh.ib.indices.data());


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

    GLVertexBuffer rectVB(4 * sizeof(Vertex), rectVertices);
    VertexBufferLayout layout;
    layout.append(2, GL_FLOAT, VariableType::FLOAT, posAttrIndex);
    layout.append(4, GL_FLOAT, VariableType::FLOAT, colAttrIndex);
    GLVertexArray rectVA;
    rectVA.addBuffer(rectVB, layout);
    GLIndexBuffer rectIB(GL_UNSIGNED_INT, 6, indices);

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

    GLVertexBuffer starVB(starVertices.size() * sizeof(Vertex), starVertices.data());
    // reuse layout from rectangle:
    GLVertexArray starVA;
    starVA.addBuffer(starVB, layout);
    GLIndexBuffer starIB(GL_UNSIGNED_INT,
                         static_cast<GLIndexBuffer::count_type>(starIndices.size()),
                         starIndices.data());

    // import from OBJ-file:
    GLShaderProgram suzanneSP(fs::path("res/shaders/Texture.shader", fs::path::format::generic_format));
    auto time_start = std::chrono::high_resolution_clock::now();
    //std::vector<CPUMesh<GLuint>> suzanneCPUMeshes = readOBJ("res/meshes/suzanne_scaled_smooth_subdiv_1_left_earring.obj", true);
    std::vector<CPUMesh<GLuint>> suzanneCPUMeshes = loadOBJfile(fs::path("res/meshes/suzanne_with_sphere_and_plane.obj", fs::path::format::generic_format),
                                                                true);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds time_delta = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    std::cout << "loading suzanne took " << time_delta.count() << " milliseconds\n";
    if (suzanneCPUMeshes.empty()) {
        std::cerr << "error no object was found in obj file\n";
        return 1;
    }

    struct GLMesh {
        GLVertexBuffer vb;
        GLVertexArray va; // contains vb but does not own it
        GLIndexBuffer ib;
    };
    std::vector<GLMesh> suzanneMeshes;
    for (auto& cpuMesh : suzanneCPUMeshes) {
        GLVertexBuffer vb(cpuMesh.va.data.size(),
                          cpuMesh.va.data.data());
        cpuMesh.va.layout.setLocations(suzanneSP);
        GLVertexArray va;
        va.addBuffer(vb, cpuMesh.va.layout);
        GLIndexBuffer ib(GL_UNSIGNED_INT,
                         static_cast<GLIndexBuffer::count_type>(cpuMesh.ib.indices.size()),
                         cpuMesh.ib.indices.data());
                         // suzanneCPUMesh.ib.primitiveType,
                         // suzanneCPUMesh.ib.primitiveRestartIndex);
        suzanneMeshes.push_back(GLMesh{std::move(vb),
                                       std::move(va),
                                       std::move(ib)});
    }

    GLTexture texture;
    int texUnit = 0;
    texture.bind(texUnit);
    suzanneSP.setUniform1i("tex", texUnit);

    GLRenderer renderer;
    renderer.setClearColor(.2f, .8f, .2f, 0.f);
    renderer.enableFaceCulling();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.disableDepthTest();

        // 0 draw house:
        shaderProgram.bind();
        shaderProgram.setUniform4f("u_Color", .5f, .5f, .5f, 1.0f);

        renderer.draw(houseVA, houseIB, shaderProgram);

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
        shaderProgram.bind();
        shaderProgram.setUniform4f("u_Color", r, .3f, .8f, 1.0f);

        renderer.draw(rectVA, rectIB, shaderProgram);


        // 2 draw star:
        shaderProgram.bind();
        shaderProgram.setUniform4f("u_Color", .8f, .8f, .8f, 1.0f);

        renderer.draw(starVA, starIB, shaderProgram);

        // 3 draw suzanne:
        renderer.enableDepthTest();
        for (auto& mesh : suzanneMeshes) {
            renderer.draw(mesh.va, mesh.ib, suzanneSP);
        }


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // glfwTerminate(); in destructor of GLFWInitialization
    return 0;
}

