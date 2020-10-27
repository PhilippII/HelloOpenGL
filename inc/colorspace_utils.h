#ifndef COLORSPACE_UTILS_H
#define COLORSPACE_UTILS_H

#include "glm/glm.hpp"

glm::vec3 sRGB_from_linRGB(const glm::vec3& rgb);

glm::vec3 linRGB_from_sRGB(const glm::vec3& sRGB);


#endif // COLORSPACE_UTILS_H
