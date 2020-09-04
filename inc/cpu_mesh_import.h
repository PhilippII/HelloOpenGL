#ifndef CPU_MESH_IMPORT_H
#define CPU_MESH_IMPORT_H

#include <filesystem>
#include "cpu_mesh_structs.h" // for CPUMesh<T>

std::vector<CPUMesh<GLuint>> loadOBJfile(std::filesystem::path filepath, bool invert_z = false);

#endif // CPU_MESH_IMPORT_H
