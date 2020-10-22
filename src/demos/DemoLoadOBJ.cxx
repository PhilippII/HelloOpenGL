#include "demos/DemoLoadOBJ.h"

#include <filesystem>

#include "debug_utils.h"

#include "imgui.h"

#include "cpu_mesh_import.h"

#include "VertexBufferLayout.h"


#include "glm/glm.hpp"


const GLuint demo::DemoLoadOBJ::texUnit = 0;


demo::DemoLoadOBJ::DemoLoadOBJ(GLRenderer &renderer)
    : demo::Demo(renderer),
      m_camera(glm::radians(45.f), 1.f, .1f, 10.f),
      m_toLight_wc(0.f, 1.f, 0.f) // must be normalized!
{
    namespace fs = std::filesystem;

    // move camera back a bit from the origin:
    m_camera.translate_global(glm::vec3(0.f, 0.f, 4.f));

    // load shader:
    m_shaderP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/TexturedPhongRefl.shader",
                                                           fs::path::format::generic_format));
    // light properties:
    // m_shaderP->setUniform3f("u_i_s", 1.f, 1.f, 1.f); -> we just set u_i_s := u_i_d
    m_shaderP->setUniform3f("u_i_d", 1.f, 1.f, 1.f);
    m_shaderP->setUniform3f("u_i_a", .5f, .5f, .5f);

    // material properties:
    m_shaderP->setUniform3f("u_k_s", .5f, .5f, .5f);
    // m_shaderP->setUniform3f("u_k_d", .8f, .2f, .8f); // -> is read from texture
    // m_shaderP->setUniform3f("u_k_a", .8f, .2f, .8f); // -> we just set u_k_a := u_k_d
    m_shaderP->setUniform1f("u_shininess", 20.f);

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

demo::DemoLoadOBJ::~DemoLoadOBJ()
{
    getRenderer().disableDepthTest();
    getRenderer().disableFaceCulling();
}

void demo::DemoLoadOBJ::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
    m_camera.setAspect(width / static_cast<float>(height));
}

void demo::DemoLoadOBJ::OnKeyPressed(int key, int scancode, int action, int mods)
{
    constexpr float stepSize = .2f;
    constexpr float rotDelta = glm::radians(5.f);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3( 0.f,  0.f, -stepSize));
    } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3( 0.f,  0.f, +stepSize));
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3(-stepSize,  0.f,  0.f));
    } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3(+stepSize,  0.f,  0.f));
    } else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        m_camera.translate_global(glm::vec3( 0.f, -stepSize,  0.f));
    } else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        m_camera.translate_global(glm::vec3( 0.f, +stepSize,  0.f));
    } else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        m_camera.rotateYaw(-rotDelta);
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        m_camera.rotateYaw(+rotDelta);
    } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        m_camera.rotatePitch(+rotDelta);
    } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        m_camera.rotatePitch(-rotDelta);
    } else if (key == GLFW_KEY_KP_DECIMAL && action == GLFW_PRESS) {
        m_camera.resetLocRot();
    }
}

void demo::DemoLoadOBJ::OnRender()
{
    getRenderer().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shaderP->bind(); // binding needed to set the uniforms
    glm::mat4 ndc_from_cc = m_camera.mat_ndc_from_cc();
    glm::mat4 cc_from_wc = m_camera.mat_cc_from_wc();
    glm::mat4 wc_from_oc(1.f);

    glm::mat4 cc_from_oc = cc_from_wc * wc_from_oc;
    glm::mat4 ndc_from_oc = ndc_from_cc * cc_from_oc;
    m_shaderP->setUniformMat4f("u_cc_from_oc", cc_from_oc);
    m_shaderP->setUniformMat4f("u_ndc_from_oc", ndc_from_oc);

    glm::vec3 toLightNormalized_wc = (m_toLight_wc == glm::vec3(0.f)) ? glm::vec3(0.f, 1.f, 0.f)
                                                                      : glm::normalize(m_toLight_wc);
    glm::vec3 toLight_cc = glm::vec3(cc_from_wc * glm::vec4(toLightNormalized_wc, 0.f));
    m_shaderP->setUniform3f("u_L_cc", toLight_cc);


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

void demo::DemoLoadOBJ::OnImGuiRender()
{
    ImGui::SliderFloat3("light direction (world space coordinates)", &m_toLight_wc.x, -1.f, 1.f);
}


