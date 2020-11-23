#include "demos/Demo.h"
#include "imgui.h"

#include <algorithm> // for std::find_if()
#include <iostream>

namespace demo {

// Demo:
void Demo::OnWindowSizeChanged(int width, int height)
{
    getRenderer().setViewport(0, 0, width, height);
}

// uncomment parameter names to avoid -Wunused-parameter warnings:
bool Demo::OnKeyPressed(int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
    return false;
}

void Demo::OnUpdate(float /*deltaSeconds*/)
{
    // do nothing by default
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

bool DemoSuite::OnKeyPressed(int key, int scancode, int action, int mods)
{
    if (m_currentDemo && m_currentDemo->OnKeyPressed(key, scancode, action, mods)) {
        return true;
    } else if (m_currentDemo && key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        m_currentDemo.reset();
        getRenderer().setClearColor();
        return true;
    } else {
        return false;
    }
}

void DemoSuite::OnUpdate(float deltaSeconds)
{
    if (m_currentDemo) {
        m_currentDemo->OnUpdate(deltaSeconds);
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

void DemoSuite::SelectDemo(std::string_view name)
{
    auto search = std::find_if(m_demos.begin(), m_demos.end(),
                               [&](const auto& value) { return value.first == name; });
    // TODO: is find_if() guaranteed to be able to handle a generic lambda?
    //       (generic = with "auto"-argument type)
    //       how exactly does this generic lambda stuff work in c++?
    if (search != m_demos.end()) {
        // clean up old demo:
        m_currentDemo.reset();
        getRenderer().setClearColor();

        // initialize new demo:
        m_currentDemo = search->second(getRenderer());
        if (m_width >= 0) {
            m_currentDemo->OnWindowSizeChanged(m_width, m_height);
        }
    } else {
        std::cout << "sorry demo " << name << " was not found. Stay in main-menu.\n";
    }
}

}
