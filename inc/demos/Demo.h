#ifndef DEMO_H
#define DEMO_H

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

}

#endif // DEMO_H
