#include "demos/DemoFramebuffer.h"

#include <filesystem>

#include "debug_utils.h"

#include "imgui.h"

#include "cpu_mesh_import.h"

#include "VertexBufferLayout.h"

#include "colorspace_utils.h"

#include "glm/glm.hpp"



const GLuint demo::DemoFramebuffer::texUnitDiffuse = 0;


demo::DemoFramebuffer::DemoFramebuffer(GLRenderer &renderer)
    : demo::Demo(renderer),
      m_camera(glm::radians(45.f), 1.f, .1f, 10.f),
      m_camereController(m_camera),
      m_clearColor_sRGB(.5f, .5f, .5f),
      // m_i_s(1.f, 1.f, 1.f), just set i_s := i_d;
      m_i_d_sRGB(1.f, 1.f, 1.f),
      m_i_a_sRGB(.5f, .5f, .5f),
      m_sunController(glm::radians(45.f), glm::radians(135.f)),
      m_k_s_sRGB(.5f, .5f, .5f),
      // m_k_d(.8f, .2f, .8f), from texture
      // m_k_a(.8f, .2f, .8f), just set k_a := k_d (:= texture color)
      m_shininess(150.f)
{
    namespace fs = std::filesystem;

    // move camera back a bit from the origin:
    m_camera.translate_global(glm::vec3(0.f, 0.f, 4.f));

    // load shader:
    m_phongReflModelSP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/TexturedPhongRefl.shader",
                                                           fs::path::format::generic_format));

    // load meshes from file:
    std::vector<CPUMesh<GLuint>> cpu_meshes = loadOBJfile(fs::path("res/meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01.obj",
                                                                   fs::path::format::generic_format));
    for (auto& cpu_mesh : cpu_meshes) {
        GLVertexBuffer vbo(cpu_mesh.va.data.size(), cpu_mesh.va.data.data());
        GLVertexArray vao;
        cpu_mesh.va.layout.setLocations(*m_phongReflModelSP);
        vao.addBuffer(vbo, cpu_mesh.va.layout);
        GLIndexBuffer ibo(GL_UNSIGNED_INT, static_cast<GLIndexBuffer::count_type>(cpu_mesh.ib.indices.size()),
                          reinterpret_cast<GLvoid*>(cpu_mesh.ib.indices.data()));
        m_glMeshes.push_back(std::tuple{std::move(vbo), std::move(vao), std::move(ibo)});
    }

    // load texture from file:
     m_texBaseColor = std::make_unique<GLTexture>(fs::path("res/meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01_Textures/GothicBed_01_8-bit_Diffuse.png",
                                                          fs::path::format::generic_format),
                                                  3, true);
     m_texBaseColor->bind(texUnitDiffuse);
     m_phongReflModelSP->bind();
     m_phongReflModelSP->setUniform1i("tex", texUnitDiffuse);

    // enable culling and depth test:
    getRenderer().enableFaceCulling();
    getRenderer().enableDepthTest();

    getRenderer().enable_framebuffer_sRGB();

    // init textures to be used in framebuffer
    // m_texColorBuffer = see OnWindowSizeChanged()
    // m_texDepthBuffer = see OnWindowSizeChanged()

    glGenFramebuffers(1, &m_fbo_id);
    // attach m_texColorBuffer and m_texColorBuffer, see OnWindowSizeChanged()
    std::array<GLenum, 1> drawBuffers = { GL_COLOR_ATTACHMENT0 };
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
    glDrawBuffers(1, drawBuffers.data());
}

demo::DemoFramebuffer::~DemoFramebuffer()
{
    glDeleteFramebuffers(1, &m_fbo_id);
    getRenderer().disable_framebuffer_sRGB();

    getRenderer().disableDepthTest();
    getRenderer().disableFaceCulling();
}

void demo::DemoFramebuffer::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
    m_camera.setAspect(static_cast<float>(width) / static_cast<float>(height));

    // clear existing attachments if any (is this necessary?):
//    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
//                           GL_TEXTURE_2D, 0, 0);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//                           GL_TEXTURE_2D, 0, 0);

    // allocate new textures of appropriate size (delete old textures if any):
    glActiveTexture(GL_TEXTURE0 + texUnitDiffuse + 1); // avoid unbinding the texture currently bound to texUnit in the constructors
                                                // of the following two textures:
    m_texColorBuffer = std::make_unique<GLTexture>(width, height, GL_RGBA32F, texture_sampling_presets::noFilter);
    m_texDepthBuffer = std::make_unique<GLTexture>(width, height, GL_DEPTH_COMPONENT16, texture_sampling_presets::noFilter);

    // attach new textures to framebuffer:
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_texColorBuffer->getRendererId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_texDepthBuffer->getRendererId(), 0);

    // check framebuffer completeness:
    myAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

bool demo::DemoFramebuffer::OnKeyPressed(int key, int scancode, int action, int mods)
{
    return m_camereController.OnKeyPressed(key, scancode, action, mods);
}

void demo::DemoFramebuffer::OnUpdate(float deltaSeconds)
{
    m_camereController.OnUpdate(deltaSeconds);
}

void demo::DemoFramebuffer::OnRender()
{
    myAssert(m_texColorBuffer); // otherwise OnWindowSizeChanged(..) has not been called yet.

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // for now
    // TODO: glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
    getRenderer().setClearColor(glm::vec4(linRGB_from_sRGB(m_clearColor_sRGB), 1.f));
    getRenderer().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set matrix uniforms:
    m_phongReflModelSP->bind(); // binding needed to set the uniforms
    glm::mat4 ndc_from_cc = m_camera.mat_ndc_from_cc();
    glm::mat4 cc_from_wc = m_camera.mat_cc_from_wc();
    glm::mat4 wc_from_oc(1.f);

    glm::mat4 cc_from_oc = cc_from_wc * wc_from_oc;
    glm::mat4 ndc_from_oc = ndc_from_cc * cc_from_oc;
    m_phongReflModelSP->setUniformMat4f("u_cc_from_oc", cc_from_oc);
    m_phongReflModelSP->setUniformMat4f("u_ndc_from_oc", ndc_from_oc);

    // set light properties uniforms:
    // m_shaderP->setUniform3f("u_i_s", m_i_s); -> we just set u_i_s := u_i_d
    m_phongReflModelSP->setUniform3f("u_i_d", linRGB_from_sRGB(m_i_d_sRGB));
    m_phongReflModelSP->setUniform3f("u_i_a", linRGB_from_sRGB(m_i_a_sRGB));

    glm::vec3 toLight_wc = m_sunController.makeToSun_wc();
    glm::vec3 toLight_cc = glm::vec3(cc_from_wc * glm::vec4(toLight_wc, 0.f));
    m_phongReflModelSP->setUniform3f("u_L_cc", toLight_cc);

    // set material properties uniforms:
    m_phongReflModelSP->setUniform3f("u_k_s", linRGB_from_sRGB(m_k_s_sRGB));
    // m_shaderP->setUniform3f("u_k_d", m_k_d); // -> is read from texture
    // m_shaderP->setUniform3f("u_k_a", m_k_a); // -> we just set u_k_a := u_k_d
    m_phongReflModelSP->setUniform1f("u_shininess", m_shininess);

    for (auto& glMesh : m_glMeshes) {
        getRenderer().draw(std::get<GLVertexArray>(glMesh),
                           std::get<GLIndexBuffer>(glMesh),
                           *m_phongReflModelSP);
        // note: while we did not need to pass the GLVertexBuffer here it was still necessary to
        //          store it. otherwise its destructor would have deallocated the vb's data
        //          on the GPU as well. But the data on the GPU is needed as it is referenced
        //          by the GLVertexArray.
    }
}

void demo::DemoFramebuffer::OnImGuiRender()
{
    // clear color:
    ImGui::ColorEdit3("clear color (sRGB)", &m_clearColor_sRGB.x);

    // light properties:
    // ImGui::ColorEdit4("i_s", &m_i_s.x);
    ImGui::Text("i_s := i_d");
    ImGui::ColorEdit3("i_d (sRGB)", &m_i_d_sRGB.x);
    ImGui::ColorEdit3("i_a (sRGB)", &m_i_a_sRGB.x);
    m_sunController.OnImGuiRender();

    // material properties:
    ImGui::ColorEdit3("k_s (sRGB)", &m_k_s_sRGB.x);
    // ImGui::ColorEdit4("k_d", &m_k_d.x);
    // ImGui::ColorEdit4("k_a", &m_k_a.x);
    ImGui::Text("k_d := k_a := texture color");
    ImGui::SliderFloat("shininess", &m_shininess, 1.f, 500.f);

    // camera controls:
    m_camereController.OnImGuiRender();
}


