#ifndef TESTTEXTURE2D_H
#define TESTTEXTURE2D_H

#include "tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory> // for std::unique_ptr

namespace test {

class TestTexture2D : public Test
{
public:
    TestTexture2D();
    ~TestTexture2D();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
private:
    static const unsigned int texSlot;

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;

    glm::mat4 m_Proj, m_View;
    glm::vec3 m_TranslationA, m_TranslationB;
};

}

#endif // TESTTEXTURE2D_H
