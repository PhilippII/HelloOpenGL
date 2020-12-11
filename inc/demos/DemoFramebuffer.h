#ifndef DEMOFRAMEBUFFER_H
#define DEMOFRAMEBUFFER_H

#include "Demo.h"

#include <vector>
#include <tuple>
#include <memory>

#include "Camera.h"
#include "ControllerCamera.h"

#include "GLVertexArray.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"

#include "GLTexture.h"

#include "GLShaderProgram.h"

#include "ControllerSun.h"

#include "GLFramebufferObject.h"


namespace demo {

class DemoFramebuffer : public Demo
{
public:
    DemoFramebuffer(GLRenderer& renderer);
    ~DemoFramebuffer();

    void OnWindowSizeChanged(int width, int height) override;
    bool OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnUpdate(float deltaSeconds) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    static const GLuint texUnitDiffuse;
    static const GLuint texUnitColorBuffer;
    static const GLuint texUnitUnused;

    // 1. members for rendering into fbo
    // ---------------------------------
    Camera m_camera;
    ControllerCamera m_camereController;

    std::unique_ptr<GLShaderProgram> m_phongReflModelSP;

    // clear color:
    glm::vec3 m_clearColor_sRGB;

    // light properties:
    // glm::vec3 m_i_s; just set i_s := i_d;
    glm::vec3 m_i_d_sRGB;
    glm::vec3 m_i_a_sRGB;
    ControllerSun m_sunController;

    // material properties:
    glm::vec3 m_k_s_sRGB;
    // glm::vec3 m_k_d; from texture
    // glm::vec3 m_k_a; just set k_a := k_d (:= texture color)
    float m_shininess;

    std::vector<std::tuple<GLVertexBuffer, GLVertexArray, GLIndexBuffer>> m_glMeshes;
    std::unique_ptr<GLTexture> m_texBaseColor;

    // 2. members for fbo
    // ------------------
    std::unique_ptr<GLTexture> m_texColorBuffer;
    std::unique_ptr<GLTexture> m_texDepthBuffer;
    std::unique_ptr<GLFramebufferObject> m_fbo;

    // 3. members for rendering from fbo to screen
    // -------------------------------------------
    std::unique_ptr<GLShaderProgram> m_filterSP;

    std::unique_ptr<GLIndexBuffer> m_rectIBO;
    std::unique_ptr<GLVertexBuffer> m_rectVBO;
    std::unique_ptr<GLVertexArray> m_rectVAO;
};

}

#endif // DEMOFRAMEBUFFER_H
