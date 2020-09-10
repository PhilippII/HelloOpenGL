#ifndef CPU_MESH_STRUCTS_H
#define CPU_MESH_STRUCTS_H

#include <vector>
#include <array>
#include <GL/glew.h>
#include <iostream>
#include <optional>

#include "VertexBufferLayout.h"

template <typename Index>
struct CPUIndexBuffer {
    std::vector<Index> indices;
    GLenum primitiveType = GL_TRIANGLES;
    std::optional<Index> primitiveRestartIndex = {};
};

// TODO: N should be unsigned int?
template <typename Index, int N>
struct CPUMultiIndexBuffer {
    std::vector<std::array<Index, N>> indices;
    GLenum primitiveType = GL_TRIANGLES;
    std::optional<std::array<Index, N>> primitiveRestartMultiIndex = {};
};

struct CPUVertexArray {
    VertexBufferLayout layout;
    std::vector<GLbyte> data;
};

template <typename Index>
struct CPUMesh {
    CPUIndexBuffer<Index> ib;
    CPUVertexArray va;
};

template <typename Index, int N>
struct CPUMultiIndexMesh {
    CPUMultiIndexBuffer<Index, N> mib;
    std::array<CPUVertexArray, N> vas;
};


template <typename Index>
std::ostream& operator<<(std::ostream& os, const CPUIndexBuffer<Index>& ib) {
    os << "CPUIndexBuffer {";
    if (!ib.indices.empty()) {
        os << ib.indices[0];
    }
    for (unsigned int i = 1; i < ib.indices.size(); ++i) {
        os << ", " << ib.indices[i];
    }
    return os << "}\n";
}

std::ostream& operator<<(std::ostream& os, const CPUVertexArray& va);

template <typename Index>
std::ostream& operator<<(std::ostream& os, const CPUMesh<Index>& cpuMesh) {
    return os << cpuMesh.ib << cpuMesh.va;
}


#endif // CPU_MESH_STRUCTS_H
