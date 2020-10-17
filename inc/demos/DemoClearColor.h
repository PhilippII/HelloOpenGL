#ifndef DEMOCLEARCOLOR_H
#define DEMOCLEARCOLOR_H

#include "Demo.h"

namespace demo {

class DemoClearColor : public Demo
{
public:
    DemoClearColor(GLRenderer& renderer);
    ~DemoClearColor();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    GLRenderer& m_renderer;
    float m_clearColor[4];
};

}

#endif // DEMOCLEARCOLOR_H
