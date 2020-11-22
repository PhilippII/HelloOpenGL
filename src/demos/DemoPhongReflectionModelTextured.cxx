#include "demos/DemoPhongReflectionModelTextured.h"

#include <filesystem>

#include "debug_utils.h"

#include "imgui.h"

#include "cpu_mesh_import.h"

#include "VertexBufferLayout.h"


#include "glm/glm.hpp"


const GLuint demo::DemoPhongReflectionModelTextured::texUnit = 0;


demo::DemoPhongReflectionModelTextured::DemoPhongReflectionModelTextured(GLRenderer &renderer)
    : demo::Demo(renderer),
      m_camera(glm::radians(45.f), 1.f, .1f, 10.f),
      m_cameraController(m_camera),
      // m_i_s(1.f, 1.f, 1.f), just set i_s := i_d;
      m_i_d(1.f, 1.f, 1.f),
      m_i_a(.5f, .5f, .5f),
      m_sunController(glm::radians(45.f), glm::radians(135.f)),
      m_k_s(.5f, .5f, .5f),
      // m_k_d(.8f, .2f, .8f), from texture
      // m_k_a(.8f, .2f, .8f), just set k_a := k_d (:= texture color)
      m_shininess(150.f)
{
    namespace fs = std::filesystem;

    // move camera back a bit from the origin:
    m_camera.translate_global(glm::vec3(0.f, 0.f, 4.f));

    // load shader:
    m_shaderP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/TexturedPhongRefl.shader",
                                                           fs::path::format::generic_format));

    // load meshes from file:
    std::vector<CPUMesh<GLuint>> cpu_meshes = loadOBJfile(fs::path("res/meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01.obj",
                                                                   fs::path::format::generic_format));
    for (auto& cpu_mesh : cpu_meshes) {
        GLVertexBuffer vbo(cpu_mesh.va.data.size(), cpu_mesh.va.data.data());
        GLVertexArray vao;
        cpu_mesh.va.layout.setLocations(*m_shaderP);
        vao.addBuffer(vbo, cpu_mesh.va.layout);
        GLIndexBuffer ibo(GL_UNSIGNED_INT, cpu_mesh.ib.indices.size(),
                          reinterpret_cast<GLvoid*>(cpu_mesh.ib.indices.data()));
        m_glMeshes.push_back(std::tuple{std::move(vbo), std::move(vao), std::move(ibo)});
    }

    // load texture from file:
     m_texBaseColor = std::make_unique<GLTexture>(fs::path("res/meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01_Textures/GothicBed_01_8-bit_Diffuse.png",
                                                          fs::path::format::generic_format), 3);
     m_texBaseColor->bind(texUnit);
     m_shaderP->bind();
     m_shaderP->setUniform1i("tex", texUnit);

    // enable culling and depth test:
    getRenderer().enableFaceCulling();
    getRenderer().enableDepthTest();
}

demo::DemoPhongReflectionModelTextured::~DemoPhongReflectionModelTextured()
{
    getRenderer().disableDepthTest();
    getRenderer().disableFaceCulling();
}

void demo::DemoPhongReflectionModelTextured::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
    m_camera.setAspect(width / static_cast<float>(height));
}

bool demo::DemoPhongReflectionModelTextured::OnKeyPressed(int key, int scancode, int action, int mods)
{
    return m_cameraController.OnKeyPressed(key, scancode, action, mods);
}

void demo::DemoPhongReflectionModelTextured::OnUpdate(float deltaSeconds)
{
    m_cameraController.OnUpdate(deltaSeconds);
}

void demo::DemoPhongReflectionModelTextured::OnRender()
{
    getRenderer().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set matrix uniforms:
    m_shaderP->bind(); // binding needed to set the uniforms
    glm::mat4 ndc_from_cc = m_camera.mat_ndc_from_cc();
    glm::mat4 cc_from_wc = m_camera.mat_cc_from_wc();
    glm::mat4 wc_from_oc(1.f);

    glm::mat4 cc_from_oc = cc_from_wc * wc_from_oc;
    glm::mat4 ndc_from_oc = ndc_from_cc * cc_from_oc;
    m_shaderP->setUniformMat4f("u_cc_from_oc", cc_from_oc);
    m_shaderP->setUniformMat4f("u_ndc_from_oc", ndc_from_oc);

    // set light properties uniforms:
    // m_shaderP->setUniform3f("u_i_s", m_i_s); -> we just set u_i_s := u_i_d
    m_shaderP->setUniform3f("u_i_d", m_i_d);
    m_shaderP->setUniform3f("u_i_a", m_i_a);

    glm::vec3 toLight_wc = m_sunController.makeToSun_wc();
    glm::vec3 toLight_cc = glm::vec3(cc_from_wc * glm::vec4(toLight_wc, 0.f));
    m_shaderP->setUniform3f("u_L_cc", toLight_cc);

    // set material properties uniforms:
    m_shaderP->setUniform3f("u_k_s", m_k_s);
    // m_shaderP->setUniform3f("u_k_d", m_k_d); // -> is read from texture
    // m_shaderP->setUniform3f("u_k_a", m_k_a); // -> we just set u_k_a := u_k_d
    m_shaderP->setUniform1f("u_shininess", m_shininess);

    for (auto& glMesh : m_glMeshes) {
        getRenderer().draw(std::get<GLVertexArray>(glMesh),
                           std::get<GLIndexBuffer>(glMesh),
                           *m_shaderP);
        // note: while we did not need to pass the GLVertexBuffer here it was still necessary to
        //          store it. otherwise its destructor would have deallocated the vb's data
        //          on the GPU as well. But the data on the GPU is needed as it is referenced
        //          by the GLVertexArray.
    }
}

void demo::DemoPhongReflectionModelTextured::OnImGuiRender()
{
    // light properties:
    // ImGui::ColorEdit4("i_s", &m_i_s.x);
    ImGui::Text("i_s := i_d");
    ImGui::ColorEdit3("i_d", &m_i_d.x);
    ImGui::ColorEdit3("i_a", &m_i_a.x);
    m_sunController.OnImGuiRender();

    // material properties:
    ImGui::ColorEdit3("k_s", &m_k_s.x);
    // ImGui::ColorEdit4("k_d", &m_k_d.x);
    // ImGui::ColorEdit4("k_a", &m_k_a.x);
    ImGui::Text("k_d := k_a := texture color");
    ImGui::SliderFloat("shininess", &m_shininess, 1.f, 500.f);

    // camera controls:
    m_cameraController.OnImGuiRender();
}



