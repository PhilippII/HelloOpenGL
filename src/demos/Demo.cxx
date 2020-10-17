#include "demos/Demo.h"
#include "imgui.h"

namespace demo {

DemoSuite::DemoSuite(GLRenderer &renderer)
    : Demo(renderer)
{
    getRenderer().setClearColor();
}

DemoSuite::~DemoSuite()
{

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
            }
        }
    }
}

}
