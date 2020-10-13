#ifndef TEST_H
#define TEST_H

#include "Renderer.h"

namespace test {

class Test
{
public:
    Test() {}
    virtual ~Test() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(Renderer& renderer) {}
    virtual void OnImGuiRender() {}
};

}

#endif // TEST_H
