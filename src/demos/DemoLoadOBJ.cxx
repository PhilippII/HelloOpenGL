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
      m_cameraController(m_camera)
{
    namespace fs = std::filesystem;

    // move camera back a bit from the origin:
    m_camera.translate_global(glm::vec3(0.f, 0.f, 4.f));

    // load shader:
    m_shaderP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/ShadelessTexture.shader",
                                                           fs::path::format::generic_format));

    // load meshes from file:
    std::vector<CPUMesh<GLuint>> cpu_meshes = loadOBJfile(fs::path("res/meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01.obj",
                                                                   fs::path::format::generic_format));
    for (auto& cpu_mesh : cpu_meshes) {
        GLVertexBuffer vbo(cpu_mesh.va.data.size(), cpu_mesh.va.data.data());
        GLVertexArray vao;
        cpu_mesh.va.layout.setLocations(*m_shaderP);
        vao.addBuffer(vbo, cpu_mesh.va.layout);
        GLIndexBuffer ibo(GL_UNSIGNED_INT, static_cast<GLIndexBuffer::count_type>(cpu_mesh.ib.indices.size()),
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
    m_camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
}

bool demo::DemoLoadOBJ::OnKeyPressed(int key, int scancode, int action, int mods)
{
    return m_cameraController.OnKeyPressed(key, scancode, action, mods);
}

void demo::DemoLoadOBJ::OnUpdate(float deltaSeconds)
{
    m_cameraController.OnUpdate(deltaSeconds);
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
    // camera controls:
    m_cameraController.OnImGuiRender();
}


