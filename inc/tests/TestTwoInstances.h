#ifndef TESTTWOINSTANCES_H
#define TESTTWOINSTANCES_H

#include "tests/Test.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include "Shader.h"

#include "Texture.h"

#include "glm/glm.hpp"


namespace test {

class TestTwoInstances : public Test
{
public:
    TestTwoInstances();
    ~TestTwoInstances();

    void OnUpdate(float deltaTime) override;
    void OnRender(Renderer& renderer) override;
    void OnImGuiRender() override;
private:
    float m_positions[4 * (2+2)];
    unsigned int m_indices[6];
    VertexArray m_va;
    VertexBuffer m_vb;
    VertexBufferLayout m_layout;
    IndexBuffer m_ib;
    Shader m_shader;
    Texture m_texture;
    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::vec3 m_translationA;
    glm::vec3 m_translationB;
};

}

#endif // TESTTWOINSTANCES_H
