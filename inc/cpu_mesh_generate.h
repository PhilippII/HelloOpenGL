#ifndef CPU_MESH_GENERATE_H
#define CPU_MESH_GENERATE_H

#include "cpu_mesh_import.h"

CPUMesh<GLuint> generateStar(int n_spikes, float r_in, float r_out, const std::array<float, 4> &centerColor, const std::array<float, 4> &spikeColor);

#endif // CPU_MESH_GENERATE_H
