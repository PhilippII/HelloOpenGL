#ifndef DEMOLOADOBJ_H
#define DEMOLOADOBJ_H

#include "Demo.h"

namespace demo {

class DemoLoadOBJ : public Demo
{
public:
    DemoLoadOBJ(GLRenderer& renderer);
    ~DemoLoadOBJ();

    void OnRender() override;
    void OnImGuiRender() override;

private:

};

}

#endif // DEMOLOADOBJ_H
