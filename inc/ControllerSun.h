#ifndef CONTROLLERSUN_H
#define CONTROLLERSUN_H

#include "glm/glm.hpp"

class ControllerSun
{
public:
    ControllerSun() : m_sunAltitute_rad(0.f), m_sunAzimuth_rad(0.f) {}
    ControllerSun(float sunAltitute_rad, float sunAzimuth_rad)
        : m_sunAltitute_rad(sunAltitute_rad), m_sunAzimuth_rad(sunAzimuth_rad)
    {}

    glm::vec3 makeToSun_wc() const;
    void OnImGuiRender();
private:
    float m_sunAltitute_rad;
    float m_sunAzimuth_rad;
};

#endif // CONTROLLERSUN_H
