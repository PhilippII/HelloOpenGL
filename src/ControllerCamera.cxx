#include "ControllerCamera.h"

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

ControllerCamera::ControllerCamera(Camera &camera)
    : m_camera(camera),
      m_forward_vel(0),
      m_backward_vel(0),
      m_left_vel(0),
      m_right_vel(0),
      m_up_vel(0),
      m_down_vel(0),
      m_yaw_left_vel(0),
      m_yaw_right_vel(0),
      m_pitch_down_vel(0),
      m_pitch_up_vel(0)
{}

bool ControllerCamera::OnKeyPressed(int key, int scancode, int action, int mods)
{
    switch (key) {
    case GLFW_KEY_W:
        updateValue(m_forward_vel, action);
        break;
    case GLFW_KEY_S:
        updateValue(m_backward_vel, action);
        break;
    case GLFW_KEY_A:
        updateValue(m_left_vel, action);
        break;
    case GLFW_KEY_D:
        updateValue(m_right_vel, action);
        break;
    case GLFW_KEY_Q:
        updateValue(m_down_vel, action);
        break;
    case GLFW_KEY_E:
        updateValue(m_up_vel, action);
        break;
    case GLFW_KEY_LEFT:
        updateValue(m_yaw_left_vel, action);
        break;
    case GLFW_KEY_RIGHT:
        updateValue(m_yaw_right_vel, action);
        break;
    case GLFW_KEY_UP:
        updateValue(m_pitch_up_vel, action);
        break;
    case GLFW_KEY_DOWN:
        updateValue(m_pitch_down_vel, action);
        break;
    case GLFW_KEY_KP_DECIMAL:
        if (action == GLFW_PRESS) {
            m_camera.resetLocRot();
        }
        break;
    default:
        // we do not know what to do with this key (ask caller to handle it for us):
        return false;
        break;
    }
    // tell the caller we have handled the key (he should NOT handle it for us):
    return true;
}

void ControllerCamera::OnUpdate(float deltaSeconds)
{
    constexpr float speed = 1.f;
    constexpr float rotSpeed = glm::radians(30.f);
    glm::vec3 localDir(static_cast<float>(m_right_vel - m_left_vel),
                       0.f,
                       static_cast<float>(m_backward_vel - m_forward_vel));
    if (localDir != glm::vec3(0.f)) {
        glm::vec3 localVelocity = speed * glm::normalize(localDir);
        m_camera.translate_local(deltaSeconds * localVelocity);
    }

    glm::vec3 globalDir(0.f,
                        static_cast<float>(m_up_vel - m_down_vel),
                        0.f);
    if (globalDir != glm::vec3(0.f)) {
        glm::vec3 globalVelocity = speed * globalDir;
        m_camera.translate_global(deltaSeconds * globalVelocity);
    }

    int yawDir = m_yaw_right_vel - m_yaw_left_vel;
    if (yawDir != 0) {
        float yawVelocity = rotSpeed * yawDir;
        m_camera.rotateYaw(deltaSeconds * yawVelocity);
    }

    int pitchDir = m_pitch_up_vel - m_pitch_down_vel;
    if (pitchDir != 0) {
        float pitchVelocity = rotSpeed * pitchDir;
        m_camera.rotatePitch(deltaSeconds * pitchVelocity);
    }
}

void ControllerCamera::updateValue(int &value, int action)
{
    if (action == GLFW_PRESS) {
        value = 1;
    } else if (action == GLFW_RELEASE) {
        value = 0;
    }
}
