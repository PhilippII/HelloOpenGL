#include "ControllerSun.h"

#include "imgui.h"

glm::vec3 ControllerSun::makeToSun_wc() const
{
    float cosAlt = glm::cos(m_sunAltitute_rad);
    return glm::vec3(cosAlt * glm::sin(m_sunAzimuth_rad),
                     glm::sin(m_sunAltitute_rad),
                     -cosAlt * glm::cos(m_sunAzimuth_rad));
}

void ControllerSun::OnImGuiRender()
{
    ImGui::SliderAngle("sun altitute (degrees)", &m_sunAltitute_rad, -90.f, +90.f);
    ImGui::SliderAngle("sun azimuth (degrees)", &m_sunAzimuth_rad, 0.f, 360.f);
}
