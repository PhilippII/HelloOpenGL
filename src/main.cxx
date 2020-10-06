#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <array>
#include <vector>

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
#include "cpu_mesh_generate.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace fs = std::filesystem;

struct Vertex {
    std::array<float, 2> pos;
    std::array<float, 4> color;
};

void error_callback(int error, const char* description) {
    std::cout << "GLFW-error [" << error << "]: " << description << '\n';
}

int main(void)
{
    #ifdef NDEBUG
    std::cout << "RELEASE VERSION\n";
    #else
    std::cout << "DEBUG VERSION\n";
    #endif
    GLFWwindow* window;

    /* Initialize GLFW */
    glfwSetErrorCallback(error_callback);
    GLFWInitialization init; // constructor calls GLFWInit()
    if (!init.getSuccess()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // for anisotropic filtering without extension
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    // initialize GLEW:
    if (glewInit() != GLEW_OK) {
        std::cout << "error: glewInit() failed!\n";
    }
    
    std::cout << glGetString(GL_VERSION) << '\n';

    // initialize transformation:
    glm::mat4 wc_from_oc(1.0f); // world coordinates from object coordinates
    glm::mat4 cc_from_wc(1.0f); // camera coordinates from world coordinates
    glm::mat4 ndc_from_cc = glm::ortho(-2.0f, 2.0f,
                                       -1.5f, 1.5f,
                                       -1.0f, 1.0f);
    glm::mat4 ndc_from_oc = ndc_from_cc * cc_from_wc * wc_from_oc;

    // initialize shader:
    GLShaderProgram shaderProgram(fs::path("res/shaders/Basic.shader", fs::path::format::generic_format));
    // shaderProgram.bind() is called automatically in constructor

    //int location = shaderProgram.getUniformLocation("u_Color");
    //myAssert(location != -1);

    shaderProgram.setUniformMat4f("u_ndc_from_oc", ndc_from_oc);

    GLint posAttrIndex = shaderProgram.getAttribLocation("position_oc");
    myAssert(posAttrIndex != -1);
    GLint colAttrIndex = shaderProgram.getAttribLocation("color");
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

    // initialize shader for textured stuff:
    GLShaderProgram texturedSP(fs::path("res/shaders/Texture.shader", fs::path::format::generic_format));
    texturedSP.setUniformMat4f("u_ndc_from_oc", ndc_from_oc);

    // initialize rectangle:
    GLTexture alphaTexture(fs::path("res/textures/alpha_texture_test.png"), 4);
    struct TexVertex {
        std::array<float, 2> pos;
        std::array<float, 2> texCoord;
    };
    TexVertex rectVertices[4] = {
        {{-.5f, -.5f}, {0.f, 0.f}}, // 0
        {{ .5f, -.5f}, {1.f, 0.f}}, // 1
        {{ .5f,  .5f}, {1.f, 1.f}}, // 2
        {{-.5f,  .5f}, {0.f, 1.f}}  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    VertexBufferLayout rectLayout;
    rectLayout.append<decltype(TexVertex::pos)>(1, "position_oc");
    rectLayout.append<decltype(TexVertex::texCoord)>(1, "texCoord");
    rectLayout.setLocations(texturedSP);

    GLVertexBuffer rectVB(4 * rectLayout.getStride(), rectVertices);
    GLVertexArray rectVA;
    rectVA.addBuffer(rectVB, rectLayout);
    GLIndexBuffer rectIB(GL_UNSIGNED_INT, 6, indices);


    // initialize star:
    float r = .0f;
    float increment = .05f;

    CPUMesh<GLuint> starCPUmesh = generateStar(5, .5f, 1.f,
                                               {0.f, 0.f, 0.f, 0.f}, {1.f, 1.f, 0.f, 1.f});
    GLVertexBuffer starVB(starCPUmesh.va.data.size(), starCPUmesh.va.data.data());
    GLVertexArray starVA;
    starCPUmesh.va.layout.setLocations(shaderProgram);
    starVA.addBuffer(starVB, starCPUmesh.va.layout);
    GLIndexBuffer starIB(GL_UNSIGNED_INT,
                         static_cast<GLIndexBuffer::count_type>(starCPUmesh.ib.indices.size()),
                         starCPUmesh.ib.indices.data());

    // import from OBJ-file:
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
        cpuMesh.va.layout.setLocations(texturedSP);
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

    GLTexture gridTexture(fs::path("res/textures/uv_grid.png", fs::path::format::generic_format));
    int texUnit = 0;
    texturedSP.setUniform1i("tex", texUnit);

    GLRenderer renderer;
    renderer.setClearColor(.2f, .8f, .2f, 0.f);
    renderer.enableFaceCulling();

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0 draw house:
        shaderProgram.bind();
        shaderProgram.setUniform4f("u_Color", .5f, .5f, .5f, 1.0f);

        renderer.draw(houseVA, houseIB, shaderProgram);

        // 2 draw star:
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
        renderer.draw(starVA, starIB, shaderProgram);


        // 3 draw suzanne:
        gridTexture.bind(texUnit);
        renderer.enableDepthTest();
        for (auto& mesh : suzanneMeshes) {
            renderer.draw(mesh.va, mesh.ib, texturedSP);
        }
        renderer.disableDepthTest();

        // 1 draw rectangle:
        GLCall(glEnable(GL_BLEND));
        alphaTexture.bind(texUnit);
        renderer.draw(rectVA, rectIB, texturedSP);
        GLCall(glDisable(GL_BLEND));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // glfwTerminate(); in destructor of GLFWInitialization
    return 0;
}

