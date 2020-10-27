#ifndef DEMOLINEARCOLORSPACE_H
#define DEMOLINEARCOLORSPACE_H

#include "Demo.h"

#include <vector>
#include <tuple>
#include <memory>

#include "Camera.h"

#include "GLVertexArray.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"

#include "GLTexture.h"

#include "GLShaderProgram.h"

namespace demo {

class DemoLinearColorspace : public Demo
{
public:
    DemoLinearColorspace(GLRenderer& renderer);
    ~DemoLinearColorspace();

    void OnWindowSizeChanged(int width, int height) override;
    void OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    static const GLuint texUnit;

    Camera m_camera;
    std::unique_ptr<GLShaderProgram> m_shaderP;

    // light properties:
    // glm::vec3 m_i_s; just set i_s := i_d;
    glm::vec3 m_i_d_sRGB;
    glm::vec3 m_i_a_sRGB;
    glm::vec3 m_toLight_wc;

    // material properties:
    glm::vec3 m_k_s_sRGB;
    // glm::vec3 m_k_d; from texture
    // glm::vec3 m_k_a; just set k_a := k_d (:= texture color)
    float m_shininess;

    std::vector<std::tuple<GLVertexBuffer, GLVertexArray, GLIndexBuffer>> m_glMeshes;
    std::unique_ptr<GLTexture> m_texBaseColor;
};

}

#endif // DEMOLINEARCOLORSPACE_H
