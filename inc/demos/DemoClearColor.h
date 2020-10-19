#ifndef DEMOCLEARCOLOR_H
#define DEMOCLEARCOLOR_H

#include "Demo.h"

namespace demo {

class DemoClearColor : public Demo
{
public:
    DemoClearColor(GLRenderer& renderer);
    ~DemoClearColor();

    void OnRender() override;
    void OnImGuiRender() override;

private:
    float m_clearColor[4];
};

}

#endif // DEMOCLEARCOLOR_H
