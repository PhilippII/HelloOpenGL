#include "tests/TestClearColor.h"

#include "imgui.h"

namespace test {

TestClearColor::TestClearColor()
    : m_ClearColor {.2f, .3f, .8f, 1.0f}
{

}

TestClearColor::~TestClearColor()
{
    GLCall(glClearColor(0.f, 0.f, 0.f, 0.f));
}

void TestClearColor::OnUpdate(float deltaTime)
{

}

void TestClearColor::OnRender(Renderer &renderer)
{
    GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void TestClearColor::OnImGuiRender()
{
    ImGui::ColorEdit4("Clear Color", m_ClearColor);
}

}
