#ifndef IMPORT_MESH_H
#define IMPORT_MESH_H

#include "cpu_mesh_structs.h" // for CPUMesh<T>
#include <string>

std::vector<CPUMesh<GLuint>> loadOBJfile(std::string filepath, bool invert_z = false);

#endif // IMPORT_MESH_H
