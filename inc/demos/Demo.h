#ifndef DEMO_H
#define DEMO_H

#include <vector>
#include <string>
#include <functional>
#include <memory> // for std::unique_ptr<..>

#include "GLRenderer.h"

namespace demo {

class Demo
{
public:
    Demo() = delete;
    Demo(GLRenderer& renderer)
        : m_renderer(renderer)
    {}
    virtual ~Demo() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}

    GLRenderer& getRenderer() { return m_renderer; }
private:
    GLRenderer& m_renderer;
};

class DemoSuite : public Demo {
public:
    DemoSuite(GLRenderer& renderer);
    ~DemoSuite();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

    template<typename T>
    void RegisterDemo(std::string name) {
        m_demos.push_back(std::pair(std::move(name),
                                    [](GLRenderer& renderer){ return std::make_unique<T>(renderer); }
                                   ));
    }
private:
    std::unique_ptr<Demo> m_currentDemo;
    std::vector<std::pair<std::string, std::function<std::unique_ptr<Demo>(GLRenderer&)>>> m_demos;
};

}

#endif // DEMO_H
