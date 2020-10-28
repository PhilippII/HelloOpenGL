#include "ControllerCamera.h"

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

bool ControllerCamera::OnKeyPressed(int key, int scancode, int action, int mods)
{
    constexpr float stepSize = .2f;
    constexpr float rotDelta = glm::radians(5.f);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3( 0.f,  0.f, -stepSize));
    } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3( 0.f,  0.f, +stepSize));
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3(-stepSize,  0.f,  0.f));
    } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        m_camera.translate_local(glm::vec3(+stepSize,  0.f,  0.f));
    } else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        m_camera.translate_global(glm::vec3( 0.f, -stepSize,  0.f));
    } else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        m_camera.translate_global(glm::vec3( 0.f, +stepSize,  0.f));
    } else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        m_camera.rotateYaw(-rotDelta);
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        m_camera.rotateYaw(+rotDelta);
    } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        m_camera.rotatePitch(+rotDelta);
    } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        m_camera.rotatePitch(-rotDelta);
    } else if (key == GLFW_KEY_KP_DECIMAL && action == GLFW_PRESS) {
        m_camera.resetLocRot();
    } else {
        // we do not know what to do with this key (ask caller to handle it for us):
        return false;
    }
    // tell the caller we have handled the key (he should NOT handle it for us):
    return true;
}

void ControllerCamera::OnUpdate(float deltaSeconds)
{

}
