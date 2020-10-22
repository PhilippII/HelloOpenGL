#ifndef DEMOPHONGREFLECTIONMODEL_H
#define DEMOPHONGREFLECTIONMODEL_H

#include "Demo.h"

#include <vector>
#include <tuple>
#include <memory>

#include "Camera.h"

#include "GLVertexArray.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"

#include "GLShaderProgram.h"

namespace demo {

class DemoPhongReflectionModel : public Demo
{
public:
    DemoPhongReflectionModel(GLRenderer& renderer);
    ~DemoPhongReflectionModel();

    void OnWindowSizeChanged(int width, int height) override;
    void OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    Camera m_camera;
    std::unique_ptr<GLShaderProgram> m_shaderP;
    glm::vec3 m_toLight_wc;
    std::vector<std::tuple<GLVertexBuffer, GLVertexArray, GLIndexBuffer>> m_glMeshes;
};

}

#endif // DEMOPHONGREFLECTIONMODEL_H
