#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"


glm::mat4 Camera::mat_ndc_from_cc() const
{
    return glm::perspective(m_fov_rad, m_aspect, m_nearClip, m_farClip);
}

void Camera::reset()
{
    m_pos_wc = glm::vec3(0.f);
    m_yaw_rad = 0.f;
    m_pitch_rad = 0.f;
    m_roll_rad = 0.f;
}

void Camera::translate_global(const glm::vec3 &offset)
{
    m_pos_wc += offset;
}

void Camera::translate_local(const glm::vec3 &offset)
{
    glm::vec4 pos_cc(offset, 1.f);
    m_pos_wc = glm::vec3(mat_wc_from_cc() * pos_cc);
}

glm::mat4 Camera::mat_wc_from_cc() const
{
    glm::mat4 res = glm::translate(glm::mat4(1.f), m_pos_wc);
    // documentation incorrect?? glm::rotate() seems to take radians while
    //  glm-documentation says it takes degrees??
    res = glm::rotate(res, m_yaw_rad, glm::vec3(0.f, -1.f, 0.f));
    res = glm::rotate(res, m_pitch_rad, glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(res, m_roll_rad, glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Camera::mat_cc_from_wc() const
{
    // TODO: invert this manually for better efficiency:
    return glm::inverse(mat_wc_from_cc());
}
