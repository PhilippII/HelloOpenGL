#include "tests/TestTexture2D.h"

#include "Renderer.h"
#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

const unsigned int TestTexture2D::texSlot = 0;

TestTexture2D::TestTexture2D()
    : m_Proj(glm::ortho(0.f, 960.f, 0.f, 540.f, -1.f, 1.f)),
      m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
{
    float positions[4 * (2+2)] = {
        -50.f, -50.f, 0.0f, 0.0f, // 0
         50.f, -50.f, 1.0f, 0.0f, // 1
         50.f,  50.f, 1.0f, 1.0f, // 2
        -50.f,  50.f, 0.0f, 1.0f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    m_VAO = std::make_unique<VertexArray>();

    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * (2+2) * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(2); // texture coordinate
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

    m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", .8f, .3f, .8f, 1.0f);

    m_Texture = std::make_unique<Texture>("res/textures/face_transparent_test.png");
    m_Shader->SetUniform1i("u_Texture", texSlot);
}

TestTexture2D::~TestTexture2D()
{

}

void TestTexture2D::OnUpdate(float deltaTime)
{

}

void TestTexture2D::OnRender()
{
    GLCall(glClearColor(0.5f, 0.5f, 0.5f, 1.f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;

    m_Texture->Bind(texSlot);

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
}

void TestTexture2D::OnImGuiRender()
{
    // TODO: SliderFloat3 wants an array but we pass a struct { float x, y, z }
    //      -> is that a problem?
    ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
    ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

}
