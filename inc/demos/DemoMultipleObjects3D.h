#ifndef DEMOMULTIPLEOBJECTS3D_H
#define DEMOMULTIPLEOBJECTS3D_H

#include "Demo.h"

namespace demo {

class DemoMultipleObjects3D : public Demo
{
public:
    DemoMultipleObjects3D(GLRenderer& renderer);
    ~DemoMultipleObjects3D();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:

};

}

#endif // DEMOMULTIPLEOBJECTS3D_H
