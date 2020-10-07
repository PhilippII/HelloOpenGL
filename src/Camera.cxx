#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "debug_utils.h"

glm::mat4 Camera::mat_ndc_from_cc() const
{
    return glm::perspective(m_fov_rad, m_aspect, m_nearClip, m_farClip);
}

void Camera::resetLocRot()
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
    glm::mat4 res(1.f);
    res = glm::translate(res, m_pos_wc);
    // 1) documentation incorrect?? glm::rotate() seems to take radians while
    //      glm-documentation says it takes degrees??
    // 2) axis-vectors for glm::rotate(..) are based on
    //      https://de.wikipedia.org/wiki/Datei:Roll_pitch_yaw_gravitation_center_de.png
    //      (yaw = Gieren, pitch=Nicken, roll=Rollen),
    //      where the coordinate system is as the eye coordinates in
    //      http://www.songho.ca/opengl/gl_projectionmatrix.html
    //      So e.g. for roll this vector points forward from the planes/pilots/cameras
    //      point of view. And forward is equivalent to the negative z-axis.
    //      Note that the direction of rotation for positive angles
    //      aligns with https://en.wikipedia.org/wiki/File:Right-hand_grip_rule.svg
    res = glm::rotate(res, m_yaw_rad,   glm::vec3( 0.f, -1.f,  0.f));
    res = glm::rotate(res, m_pitch_rad, glm::vec3(+1.f,  0.f,  0.f));
    return glm::rotate(res, m_roll_rad, glm::vec3( 0.f,  0.f, -1.f));
}

static bool almostEqual(glm::mat4 a, glm::mat4 b, float eps) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (glm::abs(a[i][j] - b[i][j]) >= eps) {
                return false;
            }
        }
    }
    return true;
}

glm::mat4 Camera::mat_cc_from_wc() const
{
    // compute inverse of code in mat_wc_from_cc():
    glm::mat4 res(1.f);
    res = glm::rotate(res, m_roll_rad,  glm::vec3( 0.f,  0.f, +1.f));
    res = glm::rotate(res, m_pitch_rad, glm::vec3(-1.f,  0.f,  0.f));
    res = glm::rotate(res, m_yaw_rad,   glm::vec3( 0.f, +1.f,  0.f));
    res = glm::translate(res, -m_pos_wc);
    myAssert(almostEqual(res, glm::inverse(mat_wc_from_cc()), .0001f));
    return res;
}
