#include "demos/DemoMultipleConcepts.h"

#include <filesystem>
#include <iostream>
#include <chrono>

#include <array>

#include <cmath> // for std::fmod()

#include "cpu_mesh_structs.h"
#include "cpu_mesh_utils.h" // to test addIndexBuffer(...)
#include "cpu_mesh_generate.h" // to test generateStar(...)
#include "cpu_mesh_import.h"

#include "VertexBufferLayout.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"

const GLuint demo::DemoMultipleConcepts::texUnit = 0;

demo::DemoMultipleConcepts::DemoMultipleConcepts(GLRenderer &renderer)
    : demo::Demo(renderer),
      m_camera(glm::radians(45.f), 1.f, .1f, 10.f),
      m_cameraController(m_camera),
      m_starColor{.5, .3f, .8f, 1.0f},
      m_starRot_deg(0.f), m_starRot_degPerSec(20.f)
{
    namespace fs = std::filesystem;

    m_camera.translate_local(glm::vec3( 0.f,  0.f, +5.f));

    // initialize shader:
    m_shaderProgram = std::make_unique<GLShaderProgram>(fs::path("res/shaders/BlendVertColUniCol.shader",
                                                                 fs::path::format::generic_format));
    // m_shaderProgram->bind() is called automatically in constructor

    //int location = m_shaderProgram->getUniformLocation("u_Color");
    //myAssert(location != -1);

    GLint posAttrIndex = m_shaderProgram->getAttribLocation("position_oc");
    myAssert(posAttrIndex != -1);
    GLint colAttrIndex = m_shaderProgram->getAttribLocation("color");
    myAssert(colAttrIndex != -1);


    // initialize house:
    struct Vertex {
        std::array<float, 2> pos;
        std::array<float, 4> color;
    };
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
    m_houseVBO = std::make_unique<GLVertexBuffer>(houseCPUMesh.va.data.size(), houseCPUMesh.va.data.data());
    m_houseVAO = std::make_unique<GLVertexArray>();
    m_houseVAO->addBuffer(*m_houseVBO, houseLayout);
    m_houseIBO = std::make_unique<GLIndexBuffer>(GL_UNSIGNED_INT,
                                                 static_cast<GLIndexBuffer::count_type>(houseCPUMesh.ib.indices.size()),
                                                 houseCPUMesh.ib.indices.data());

    // initialize shader for textured stuff:
    m_texturedSP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/ShadelessTexture.shader",
                                                              fs::path::format::generic_format));

    // initialize rectangle:
    m_alphaTexture = std::make_unique<GLTexture>(fs::path("res/textures/alpha_texture_test.png",
                                                          fs::path::format::generic_format),
                                                 4);
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
    rectLayout.setLocations(*m_texturedSP);

    m_rectVBO = std::make_unique<GLVertexBuffer>(4 * rectLayout.getStride(), rectVertices);
    m_rectVAO = std::make_unique<GLVertexArray>();
    m_rectVAO->addBuffer(*m_rectVBO, rectLayout);
    m_rectIBO = std::make_unique<GLIndexBuffer>(GL_UNSIGNED_INT, 6, indices);


    // initialize star:
    CPUMesh<GLuint> starCPUmesh = generateStar(5, .5f, 1.f,
                                               {0.f, 0.f, 0.f, 0.f}, {1.f, 1.f, 0.f, 1.f});
    m_starVBO = std::make_unique<GLVertexBuffer>(starCPUmesh.va.data.size(), starCPUmesh.va.data.data());
    m_starVAO = std::make_unique<GLVertexArray>();
    starCPUmesh.va.layout.setLocations(*m_shaderProgram);
    m_starVAO->addBuffer(*m_starVBO, starCPUmesh.va.layout);
    m_starIBO = std::make_unique<GLIndexBuffer>(GL_UNSIGNED_INT,
                                                static_cast<GLIndexBuffer::count_type>(starCPUmesh.ib.indices.size()),
                                                starCPUmesh.ib.indices.data());

    // import from OBJ-file:
    auto time_start = std::chrono::high_resolution_clock::now();
    //std::vector<CPUMesh<GLuint>> suzanneCPUMeshes = readOBJ("res/meshes/suzanne_scaled_smooth_subdiv_1_left_earring.obj", true);
    std::vector<CPUMesh<GLuint>> suzanneCPUMeshes = loadOBJfile(fs::path("res/meshes/suzanne_horn_smooth_subdiv_1.obj", fs::path::format::generic_format),
                                                                false);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds time_delta = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    std::cout << "loading suzanne took " << time_delta.count() << " milliseconds\n";
    if (suzanneCPUMeshes.empty()) {
        std::cerr << "error no object was found in obj file\n";
        myAssert(false);
    }

    for (auto& cpuMesh : suzanneCPUMeshes) {
        GLVertexBuffer vb(cpuMesh.va.data.size(),
                          cpuMesh.va.data.data());
        cpuMesh.va.layout.setLocations(*m_texturedSP);
        GLVertexArray va;
        va.addBuffer(vb, cpuMesh.va.layout);
        GLIndexBuffer ib(GL_UNSIGNED_INT,
                         static_cast<GLIndexBuffer::count_type>(cpuMesh.ib.indices.size()),
                         cpuMesh.ib.indices.data());
                         // suzanneCPUMesh.ib.primitiveType,
                         // suzanneCPUMesh.ib.primitiveRestartIndex);
        m_suzanneMeshes.push_back(GLMesh{std::move(vb),
                                         std::move(va),
                                         std::move(ib)});
    }

    m_gridTexture = std::make_unique<GLTexture>(fs::path("res/textures/uv_grid.png",
                                                         fs::path::format::generic_format));
    m_texturedSP->setUniform1i("tex", texUnit);


    getRenderer().setClearColor(.2f, .8f, .2f, 0.f);
    getRenderer().enableFaceCulling();
    getRenderer().enableDepthTest();

    getRenderer().setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

demo::DemoMultipleConcepts::~DemoMultipleConcepts()
{
    getRenderer().setClearColor();
    getRenderer().disableFaceCulling();
    getRenderer().disableDepthTest();

    getRenderer().setBlendFunc();
}

void demo::DemoMultipleConcepts::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
    m_camera.setAspect(width / static_cast<float>(height));
}

bool demo::DemoMultipleConcepts::OnKeyPressed(int key, int scancode, int action, int mods)
{
    return m_cameraController.OnKeyPressed(key, scancode, action, mods);
}

void demo::DemoMultipleConcepts::OnUpdate(float deltaSeconds)
{
    m_starRot_deg += m_starRot_degPerSec * deltaSeconds;
    m_starRot_deg = std::fmod(m_starRot_deg, 360.f);

    m_cameraController.OnUpdate(deltaSeconds);
}

void demo::DemoMultipleConcepts::OnRender()
{
    // initialize transformation:
    glm::mat4 cc_from_wc = m_camera.mat_cc_from_wc(); // camera coordinates from world coordinates
    glm::mat4 ndc_from_cc = m_camera.mat_ndc_from_cc();
    glm::mat4 ndc_from_wc = ndc_from_cc * cc_from_wc;

    /* Render here */
    getRenderer().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 0 draw house:
    m_shaderProgram->bind(); // must be bound first to set a uniform
    glm::mat4 wc_from_houseoc(1.f);
    m_shaderProgram->setUniformMat4f("u_ndc_from_oc", ndc_from_wc * wc_from_houseoc);
    m_shaderProgram->setUniform4f("u_Color", .5f, .5f, .5f, 1.0f);

    getRenderer().draw(*m_houseVAO, *m_houseIBO, *m_shaderProgram);

    // 2 draw star:
    m_shaderProgram->bind(); // must be bound first to set a uniform
    glm::mat4 wc_from_staroc(1.f);
    wc_from_staroc = glm::rotate(wc_from_staroc, glm::radians(m_starRot_deg), glm::vec3(0.f, 0.f, 1.f));
    wc_from_staroc = glm::translate(wc_from_staroc, glm::vec3(0.f, 0.f, .5f));
    m_shaderProgram->setUniformMat4f("u_ndc_from_oc", ndc_from_wc * wc_from_staroc);
    m_shaderProgram->setUniform4fv("u_Color", m_starColor);
    getRenderer().draw(*m_starVAO, *m_starIBO, *m_shaderProgram);

    // 3 draw suzanne:
    m_texturedSP->bind(); // must be bound first to set a uniform
    glm::mat4 wc_from_suzanneoc = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 1.f));
    m_texturedSP->setUniformMat4f("u_ndc_from_oc", ndc_from_wc * wc_from_suzanneoc);
    m_gridTexture->bind(texUnit);
    for (auto& mesh : m_suzanneMeshes) {
        getRenderer().draw(mesh.va, mesh.ib, *m_texturedSP);
    }

    // 1 draw rectangle:
    getRenderer().enableBlending();
    m_texturedSP->bind(); // must be bound first to set a uniform
    glm::mat4 wc_from_rectoc = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 2.f));
    m_texturedSP->setUniformMat4f("u_ndc_from_oc", ndc_from_wc * wc_from_rectoc);
    m_alphaTexture->bind(texUnit);
    getRenderer().draw(*m_rectVAO, *m_rectIBO, *m_texturedSP);
    getRenderer().disableBlending();
}

void demo::DemoMultipleConcepts::OnImGuiRender()
{
    ImGui::ColorEdit4("Star Color", m_starColor);

    // camera controls:
    m_cameraController.OnImGuiRender();
}


