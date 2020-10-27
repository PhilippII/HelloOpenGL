#include "colorspace_utils.h"

// sources:
// https://en.wikipedia.org/wiki/SRGB#Specification_of_the_transformation
// http://www.pbr-book.org/3ed-2018/Texture/Image_Texture.html#TextureMemoryManagement

static float gammaCorrect(float u) {
    if (u <= 0.0031308f) {
        return 12.92f * u;
    } else {
        return 1.055f * glm::pow(u, 1.f/2.4f) - 0.055f;
    }
}

static float inverseGammaCorrect(float u) {
    if (u <= 0.04045f) {
        return (1.f/12.92f) * u;
    } else {
        return glm::pow((u + 0.055f) * (1.f / 1.055f), 2.4f);
    }
}

glm::vec3 sRGB_from_linRGB(const glm::vec3 &rgb)
{
    return glm::vec3(gammaCorrect(rgb.r),
                     gammaCorrect(rgb.g),
                     gammaCorrect(rgb.b));
}

glm::vec3 linRGB_from_sRGB(const glm::vec3 &sRGB)
{
    return glm::vec3(inverseGammaCorrect(sRGB.r),
                     inverseGammaCorrect(sRGB.g),
                     inverseGammaCorrect(sRGB.b));
}
