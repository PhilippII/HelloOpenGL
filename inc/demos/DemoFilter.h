#ifndef DEMOFILTER_H
#define DEMOFILTER_H

#include "Demo.h"

#include <memory>

#include "GLIndexBuffer.h"
#include "GLVertexBuffer.h"
#include "GLVertexArray.h"

#include "GLTexture.h"

#include "GLShaderProgram.h"

#include "GLRenderer.h"

namespace demo {

class DemoFilter : public Demo
{
public:
    DemoFilter(GLRenderer& renderer);

    void OnRender() override;
private:
    static constexpr int texUnit {0};

    std::unique_ptr<GLShaderProgram> m_shaderP;

    std::unique_ptr<GLIndexBuffer> m_ibo;
    std::unique_ptr<GLVertexBuffer> m_vbo;
    std::unique_ptr<GLVertexArray> m_vao;

    std::unique_ptr<GLTexture> m_texture;
};

}

#endif // DEMOFILTER_H
