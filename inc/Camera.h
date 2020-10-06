#ifndef CAMERA_H
#define CAMERA_H

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class Camera
{
public:
    Camera(float fov_rad, float aspect, float nearClip, float farClip)
        : m_pos_wc(0.f), m_yaw_rad(0.f), m_pitch_rad(0.f), m_roll_rad(0.f),
          m_fov_rad(fov_rad), m_aspect(aspect), m_nearClip(nearClip), m_farClip(farClip)
    {}
    void setAspect(float aspect) { m_aspect = aspect; }

    void resetLocRot();
    // positive angles for yaw, pitch or roll correspond to
    // the directions shown in:
    // https://de.wikipedia.org/wiki/Datei:Roll_pitch_yaw_gravitation_center_de.png
    void rotateYaw(float delta_rad) { m_yaw_rad += delta_rad; }
    void rotatePitch(float delta_rad) { m_pitch_rad += delta_rad; }
    void rotateRoll(float delta_rad) { m_roll_rad += delta_rad; }

    void translate_global(const glm::vec3& offset);
    void translate_local(const glm::vec3& offset);

    glm::mat4 mat_wc_from_cc() const;
    glm::mat4 mat_cc_from_wc() const;
    glm::mat4 mat_ndc_from_cc() const;
private:
    glm::vec3 m_pos_wc;
    float m_yaw_rad;
    float m_pitch_rad;
    float m_roll_rad;

    float m_fov_rad;
    float m_aspect;
    float m_nearClip;
    float m_farClip;
};

#endif // CAMERA_H
