#ifndef CONTROLLERCAMERA_H
#define CONTROLLERCAMERA_H


#include "Camera.h"


class ControllerCamera
{
public:
    ControllerCamera() = delete;
    ControllerCamera(Camera& camera) : m_camera(camera) {}

    bool OnKeyPressed(int key, int scancode, int action, int mods);
    void OnUpdate(float deltaSeconds);
private:
    Camera& m_camera;
};

#endif // CONTROLLERCAMERA_H
