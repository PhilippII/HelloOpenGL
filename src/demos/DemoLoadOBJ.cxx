#include "demos/DemoLoadOBJ.h"

#include "debug_utils.h"

#include "imgui.h"

demo::DemoLoadOBJ::DemoLoadOBJ(GLRenderer &renderer)
    : demo::Demo(renderer)
{

}

demo::DemoLoadOBJ::~DemoLoadOBJ()
{

}

void demo::DemoLoadOBJ::OnRender()
{
    getRenderer().clear(GL_COLOR_BUFFER_BIT);
}

void demo::DemoLoadOBJ::OnImGuiRender()
{

}


