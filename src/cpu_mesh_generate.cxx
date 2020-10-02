#include "cpu_mesh_generate.h"

#include <cmath> // M_PI and cos(...) sin(...)

struct ColoredVertex {
    std::array<float, 2> pos;
    std::array<float, 4> color;
};

CPUMesh<GLuint> generateStar(int n_spikes, float r_in, float r_out, const std::array<float, 4>& centerColor, const std::array<float, 4>& spikeColor)
{
    constexpr float pi_f = static_cast<float>(M_PI);
    float dPhi = pi_f / n_spikes; // 2*M_PI / (2*n_spikes)
    float phi_0 = .5f * pi_f - dPhi;
    std::vector<float> radii {r_in, r_out};
    std::array<std::array<float, 4>, 2> colors {centerColor, spikeColor};
    std::vector<ColoredVertex> starVertices;
    for (int i = 0; i < 2 * n_spikes; ++i) {
        float phi_i = phi_0 + i * dPhi;
        float radius = radii[i % 2];
        starVertices.push_back(ColoredVertex{
                                    std::array<float, 2>{radius * cosf(phi_i), radius * sinf(phi_i)},
                                    colors[i % 2]
                               });
    }
    starVertices.push_back(ColoredVertex{
                                std::array<float, 2>{0.f, 0.f},
                                centerColor
                           });

    std::vector<GLuint> starIndices;
    for (int s = 0; s < n_spikes; ++s) {
        // inner triangle:
        starIndices.push_back(2 * s);
        starIndices.push_back((2 * s + 2) % (n_spikes * 2));
        starIndices.push_back(n_spikes * 2); // center point
        // outer triangle:
        starIndices.push_back(2 * s);
        starIndices.push_back(2 * s + 1);
        starIndices.push_back((2 * s + 2) % (n_spikes * 2));
    }

    VertexBufferLayout starLayout;
    starLayout.append(2, GL_FLOAT, VariableType::FLOAT, "position_oc");
    starLayout.append(4, GL_FLOAT, VariableType::FLOAT, "color");
    CPUVertexArray va {starLayout, std::vector<GLbyte>(reinterpret_cast<GLbyte*>(starVertices.data()),
                                                       reinterpret_cast<GLbyte*>(starVertices.data()) + starVertices.size() * sizeof(ColoredVertex))};
    CPUIndexBuffer<GLuint> ib {starIndices};
    return CPUMesh<GLuint> {ib, va};
}
