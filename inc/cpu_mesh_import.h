#ifndef CPU_MESH_IMPORT_H
#define CPU_MESH_IMPORT_H

#include "cpu_mesh_structs.h" // for CPUMesh<T>
#include <string>

std::vector<CPUMesh<GLuint>> loadOBJfile(std::string filepath, bool invert_z = false);

#endif // CPU_MESH_IMPORT_H
