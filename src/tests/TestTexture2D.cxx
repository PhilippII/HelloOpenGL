#include "tests/TestTexture2D.h"

#include "Renderer.h"
#include "imgui.h"

namespace test {

TestTexture2D::TestTexture2D()
    : m_ClearColor {.2f, .3f, .8f, 1.0f}
{

}

TestTexture2D::~TestTexture2D()
{

}

void TestTexture2D::OnUpdate(float deltaTime)
{

}

void TestTexture2D::OnRender()
{
    GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void TestTexture2D::OnImGuiRender()
{
    ImGui::ColorEdit4("Clear Color", m_ClearColor);
}

}
