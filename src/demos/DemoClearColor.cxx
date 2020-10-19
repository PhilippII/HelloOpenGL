#include "demos/DemoClearColor.h"

#include "debug_utils.h"

#include "imgui.h"

demo::DemoClearColor::DemoClearColor(GLRenderer &renderer)
    : demo::Demo(renderer), m_clearColor{.5f, .5f, .5f, 1.f}
{
}

demo::DemoClearColor::~DemoClearColor()
{
    getRenderer().setClearColor(); // reset to default value
}

void demo::DemoClearColor::OnRender()
{
    getRenderer().setClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    getRenderer().clear(GL_COLOR_BUFFER_BIT);
}

void demo::DemoClearColor::OnImGuiRender()
{
    ImGui::ColorEdit4("Clear Color", m_clearColor);
}


