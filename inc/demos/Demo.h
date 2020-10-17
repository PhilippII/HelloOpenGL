#ifndef DEMO_H
#define DEMO_H

#include "GLRenderer.h"

namespace demo {

class Demo
{
public:
    Demo() {}
    virtual ~Demo() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
};

}

#endif // DEMO_H
