#ifndef DEMOMULTIPLEOBJECTS3D_H
#define DEMOMULTIPLEOBJECTS3D_H

#include "Demo.h"

#include <memory> // for std::unique_ptr

#include "Camera.h"
#include "GLShaderProgram.h"

#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLVertexArray.h"

#include "GLTexture.h"


namespace demo {

class DemoMultipleObjects3D : public Demo
{
public:
    DemoMultipleObjects3D(GLRenderer& renderer);
    ~DemoMultipleObjects3D();

    void OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    static const GLuint texUnit;

    struct GLMesh {
        GLVertexBuffer vb;
        GLVertexArray va; // contains vb but does not own it
        GLIndexBuffer ib;
    };
    Camera m_camera;

    std::unique_ptr<GLShaderProgram> m_shaderProgram;
    std::unique_ptr<GLVertexBuffer> m_houseVBO;
    std::unique_ptr<GLVertexArray> m_houseVAO;
    std::unique_ptr<GLIndexBuffer> m_houseIBO;

    std::unique_ptr<GLShaderProgram> m_texturedSP;
    std::unique_ptr<GLTexture> m_alphaTexture;
    std::unique_ptr<GLVertexBuffer> m_rectVBO;
    std::unique_ptr<GLVertexArray> m_rectVAO;
    std::unique_ptr<GLIndexBuffer> m_rectIBO;

    std::unique_ptr<GLVertexBuffer> m_starVBO;
    std::unique_ptr<GLVertexArray> m_starVAO;
    std::unique_ptr<GLIndexBuffer> m_starIBO;

    std::vector<GLMesh> m_suzanneMeshes;
    std::unique_ptr<GLTexture> m_gridTexture;
    float m_red;
    float m_red_increment;
};

}

#endif // DEMOMULTIPLEOBJECTS3D_H
