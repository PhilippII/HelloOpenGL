#ifndef CONTROLLERCAMERA_H
#define CONTROLLERCAMERA_H


#include "Camera.h"


class ControllerCamera
{
public:
    ControllerCamera() = delete;
    ControllerCamera(Camera& camera);

    bool OnKeyPressed(int key, int scancode, int action, int mods);
    void OnUpdate(float deltaSeconds);
    void OnImGuiRender();
private:
    void updateValue(int &value, int action);
    Camera& m_camera;
    int m_forward_vel;
    int m_backward_vel;

    int m_left_vel;
    int m_right_vel;

    int m_up_vel;
    int m_down_vel;

    int m_yaw_left_vel;
    int m_yaw_right_vel;
    int m_pitch_down_vel;
    int m_pitch_up_vel;
};

#endif // CONTROLLERCAMERA_H
