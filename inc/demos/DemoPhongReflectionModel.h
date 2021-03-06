#ifndef DEMOPHONGREFLECTIONMODEL_H
#define DEMOPHONGREFLECTIONMODEL_H

#include "Demo.h"

#include <vector>
#include <tuple>
#include <memory>

#include "Camera.h"
#include "ControllerCamera.h"

#include "GLVertexArray.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"

#include "GLShaderProgram.h"

#include "ControllerSun.h"

namespace demo {

class DemoPhongReflectionModel : public Demo
{
public:
    DemoPhongReflectionModel(GLRenderer& renderer);
    ~DemoPhongReflectionModel();

    void OnWindowSizeChanged(int width, int height) override;
    bool OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnUpdate(float deltaSeconds) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    Camera m_camera;
    ControllerCamera m_cameraController;
    std::unique_ptr<GLShaderProgram> m_shaderP;

    // light properties:
    glm::vec3 m_i_s;
    glm::vec3 m_i_d;
    glm::vec3 m_i_a;
    ControllerSun m_sunController;

    // material properties:
    glm::vec3 m_k_s;
    glm::vec3 m_k_d;
    glm::vec3 m_k_a;
    float m_shininess;

    std::vector<std::tuple<GLVertexBuffer, GLVertexArray, GLIndexBuffer>> m_glMeshes;
};

}

#endif // DEMOPHONGREFLECTIONMODEL_H
