#ifndef DEMOMULTIPLECONCEPTS_H
#define DEMOMULTIPLECONCEPTS_H

// This demo demonstrates multiple different concepts, which should probably be
// separated into multiple demos:
// 1) the house in the background demonstrates the ability to remove redundancy from a vertex buffer
//      by automatically computing an index buffer
// 2) the star demonstrates the ability to animate stuff (its rotation) using the deltaSeconds
//      passed to OnUpdate()
// 3) the star also demonstrates the ability to change values (its color) interactively via dear-imgui
// 4) suzanne (the monkeyhead) demonstrates the ability to load a mesh from an WavefrontOBJ-file
// 5) suzanne and the plane (with the face) on the front demonstrate the ability to load and use
//      textures from png-files (with and without the alpha channel)
// 6) the plane (with the face) demonstrates some opengl-blending settings that actually
//      make use of the texture's alpha channel


#include "Demo.h"

#include <memory> // for std::unique_ptr

#include "Camera.h"
#include "GLShaderProgram.h"

#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLVertexArray.h"

#include "GLTexture.h"


namespace demo {

class DemoMultipleConcepts : public Demo
{
public:
    DemoMultipleConcepts(GLRenderer& renderer);
    ~DemoMultipleConcepts();

    void OnWindowSizeChanged(int width, int height) override;
    void OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnUpdate(float deltaSeconds) override;
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
    float m_starColor[4];
    float m_starRot_deg;
    float m_starRot_degPerSec;
};

}

#endif // DEMOMULTIPLECONCEPTS_H
