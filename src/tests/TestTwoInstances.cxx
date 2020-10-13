#include "tests/TestTwoInstances.h"

#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"

test::TestTwoInstances::TestTwoInstances()
    : m_positions {
          -50.f, -50.f, 0.0f, 0.0f, // 0
           50.f, -50.f, 1.0f, 0.0f, // 1
           50.f,  50.f, 1.0f, 1.0f, // 2
          -50.f,  50.f, 0.0f, 1.0f  // 3
      },
      m_indices {
          0, 1, 2,
          2, 3, 0
      },
      m_va(),
      m_vb(m_positions, 4 * (2+2) * sizeof(float)),
      m_layout(),
      m_ib(m_indices, 6),
      m_shader("res/shaders/Basic.shader"),
      m_texture("res/textures/face_transparent_test.png"),
      m_proj(glm::ortho(0.f, 960.f, 0.f, 540.f, -1.f, 1.f)),
      m_view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      m_translationA(200, 200, 0),
      m_translationB(400, 200, 0)
{
    m_layout.Push<float>(2); // position
    m_layout.Push<float>(2); // texture coordinate
    m_va.AddBuffer(m_vb, m_layout);
    constexpr unsigned int texSlot = 0;
    m_texture.Bind(texSlot);
    m_shader.Bind();
    m_shader.SetUniform1i("u_Texture", texSlot);
    GLCall(glClearColor(.5f, .5f, .5f, 1.f)); // TODO: maybe put this in Renderer class
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

test::TestTwoInstances::~TestTwoInstances()
{
    GLCall(glClearColor(0.f, 0.f, 0.f, 0.f));
    GLCall(glDisable(GL_BLEND));
    GLCall(glBlendFunc(GL_ONE, GL_ZERO));
}

void test::TestTwoInstances::OnUpdate(float deltaTime)
{

}

void test::TestTwoInstances::OnRender(Renderer &renderer)
{
    renderer.Clear();
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
        glm::mat4 mvp = m_proj * m_view * model;
        m_shader.Bind();
        m_shader.SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(m_va, m_ib, m_shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationB);
        glm::mat4 mvp = m_proj * m_view * model;
        m_shader.Bind();
        m_shader.SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(m_va, m_ib, m_shader);
    }
}

void test::TestTwoInstances::OnImGuiRender()
{
    ImGui::SliderFloat3("Translation A", &m_translationA.x, 0.0f, 960.0f);
    ImGui::SliderFloat3("Translation B", &m_translationB.x, 0.0f, 960.0f);
}
