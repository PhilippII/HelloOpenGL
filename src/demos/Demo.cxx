#include "demos/Demo.h"
#include "imgui.h"

namespace demo {

// Demo:
void Demo::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
}

// DemoSuite:
DemoSuite::DemoSuite(GLRenderer &renderer)
    : Demo(renderer),
      m_width(-1), m_height(0)
{
    getRenderer().setClearColor();
}

DemoSuite::~DemoSuite()
{

}

void DemoSuite::OnWindowSizeChanged(int width, int height)
{
    m_width = width;
    m_height = height;

    if (m_currentDemo) {
        m_currentDemo->OnWindowSizeChanged(width, height);
    } else {
        getRenderer().setViewport(0, 0, width, height);
    }
}

void DemoSuite::OnKeyPressed(int key, int scancode, int action, int mods)
{
    if (m_currentDemo) {
        m_currentDemo->OnKeyPressed(key, scancode, action, mods);
    }
}

void DemoSuite::OnUpdate(float deltaTime)
{
    if (m_currentDemo) {
        m_currentDemo->OnUpdate(deltaTime);
    }
}

void DemoSuite::OnRender()
{
    if (m_currentDemo) {
        m_currentDemo->OnRender();
    } else {
        getRenderer().clear(GL_COLOR_BUFFER_BIT);
    }
}

void DemoSuite::OnImGuiRender()
{
    if (m_currentDemo) {
        if (ImGui::Button("<-")) {
            m_currentDemo.reset();
            getRenderer().setClearColor();
        } else {
            m_currentDemo->OnImGuiRender();
        }
    } else {
        for (auto& p : m_demos) {
            if (ImGui::Button(p.first.c_str())) {
                m_currentDemo = p.second(getRenderer());
                if (m_width >= 0) {
                    m_currentDemo->OnWindowSizeChanged(m_width, m_height);
                }
            }
        }
    }
}

}
