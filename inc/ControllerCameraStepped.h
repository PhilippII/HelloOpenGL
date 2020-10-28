#ifndef CONTROLLERCAMERASTEPPED_H
#define CONTROLLERCAMERASTEPPED_H

#include "Camera.h"

class ControllerCameraStepped
{
public:
    ControllerCameraStepped() = delete;
    ControllerCameraStepped(Camera& camera) : m_camera(camera) {}

    bool OnKeyPressed(int key, int scancode, int action, int mods);
    void OnUpdate(float deltaSeconds);
private:
    Camera& m_camera;
};

#endif // CONTROLLERCAMERASTEPPED_H
