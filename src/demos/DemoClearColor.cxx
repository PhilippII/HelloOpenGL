#include "demos/DemoClearColor.h"

#include "debug_utils.h"

#include "imgui.h"

demo::DemoClearColor::DemoClearColor(GLRenderer &renderer)
    : m_renderer(renderer), m_clearColor{.5f, .5f, .5f, 1.f}
{
}

demo::DemoClearColor::~DemoClearColor()
{
    m_renderer.setClearColor(); // reset to default value
}

void demo::DemoClearColor::OnUpdate(float deltaTime)
{

}

void demo::DemoClearColor::OnRender()
{
    m_renderer.setClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    m_renderer.clear(GL_COLOR_BUFFER_BIT);
}

void demo::DemoClearColor::OnImGuiRender()
{
    ImGui::SliderFloat4("Clear Color", m_clearColor, 0.0f, 1.0f);
}


