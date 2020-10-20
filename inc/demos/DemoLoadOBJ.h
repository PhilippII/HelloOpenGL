#ifndef DEMOLOADOBJ_H
#define DEMOLOADOBJ_H

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

class DemoLoadOBJ : public Demo
{
public:
    DemoLoadOBJ(GLRenderer& renderer);
    ~DemoLoadOBJ();

    void OnWindowSizeChanged(int width, int height) override;
    void OnKeyPressed(int key, int scancode, int action, int mods) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    static const GLuint texUnit;

    Camera m_camera;
    std::vector<std::tuple<GLVertexBuffer, GLVertexArray, GLIndexBuffer>> m_glMeshes;
    std::unique_ptr<GLTexture> m_texBaseColor;
    std::unique_ptr<GLShaderProgram> m_shaderP;
};

}

#endif // DEMOLOADOBJ_H
