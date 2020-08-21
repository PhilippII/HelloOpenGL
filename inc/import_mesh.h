#ifndef IMPORT_MESH_H
#define IMPORT_MESH_H

#include <string>
#include <vector>
#include <array>
#include <GL/glew.h>
#include <iostream>

#include "VertexBufferLayout.h"

template <typename Index>
struct CPUIndexBuffer {
    std::vector<Index> indices;
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

// TODO: N should be unsigned int?
template <typename Index, int N>
struct CPUMultiIndexBuffer {
    std::vector<std::array<Index, N>> indices;
};

struct CPUVertexArray {
    VertexBufferLayout layout;
    std::vector<GLbyte> data;
};

std::ostream& operator<<(std::ostream& os, const CPUVertexArray& va);

template <typename Index>
struct CPUMesh {
    CPUIndexBuffer<Index> ib;
    CPUVertexArray va;
};

template <typename Index>
std::ostream& operator<<(std::ostream& os, const CPUMesh<Index>& cpuMesh) {
    return os << cpuMesh.ib << cpuMesh.va;
}


template <typename Index, int N>
struct CPUMultiIndexMesh {
    CPUMultiIndexBuffer<Index, N> mib;
    std::array<CPUVertexArray, N> vas;
};

template <typename Index>
CPUMesh<Index> addIndexBuffer(VertexBufferLayout layout, GLsizei count, const GLbyte* data) {
    // TODO:
    // - loop over stride is confusing
    // - linear search over i_out has bad performance
    CPUMesh<Index> res;
    res.va.layout = layout;
    auto stride = layout.getStride();
    GLsizei copiedCount = 0;
    for (unsigned int i_in = 0; i_in < count; ++i_in) {
        bool found = false;
        myAssert(copiedCount == res.va.data.size() / stride);
        unsigned int i_out;
        for (i_out = 0; i_out < copiedCount && !found; ++i_out) {
            bool equal = true;
            for (unsigned int offset = 0; offset < stride && equal; ++offset) {
                if (data[i_in * stride + offset] != res.va.data[i_out * stride + offset]) {
                    equal = false;
                }
            }
            if (equal) {
                found = true;
            }
        }
        if (found) {
            res.ib.indices.push_back(i_out - 1);
        } else {
            for (unsigned int offset = 0; offset < stride; ++offset) {
                res.va.data.push_back(data[i_in * stride + offset]);
            }
            ++copiedCount;
            res.ib.indices.push_back(copiedCount - 1);
        }
    }
    return res;
}

template <typename Index>
CPUMesh<Index> addIndexBuffer(const CPUVertexArray& va) {
    return addIndexBuffer<Index>(va.layout,
                                 va.data.size() / va.layout.getStride(),
                                 va.data.data());
}

template <typename Index, int N>
CPUVertexArray applyMultiIndex(std::size_t count, // std::vector<...>::size_type does not work for some reason
                               const std::array<Index, N>* multiIndices, const std::array<CPUVertexArray, N>& vas) {
    CPUVertexArray res;
    for (std::size_t i_v = 0; i_v < count; ++i_v) {
        // TODO: i_buff and N should be unsigned int ?
        for (int i_buf = 0; i_buf < N; ++i_buf) {
            VertexBufferLayout::stride_type stride_buf = vas[i_buf].layout.getStride();
            Index index = multiIndices[i_v][i_buf];
            for (VertexBufferLayout::stride_type offset = 0; offset < stride_buf; ++offset) {
                res.data.push_back(vas[i_buf].data[stride_buf * index + offset]);
            }
        }
    }

    for (auto& va : vas) {
        res.layout += va.layout;
    }

    return res;
}

template<typename Index, int N>
CPUVertexArray applyMultiIndex(const CPUMultiIndexMesh<Index, N>& miMesh) {
    return applyMultiIndex<Index, N>(miMesh.mib.indices.size(),
                                     miMesh.mib.indices.data(),
                                     miMesh.vas);
}
/*
template <typename Index, int N>
CPUMesh unifyIndexBuffer(CPUMultiIndexMesh miMesh) {
    VertexBufferLayout mibLayout;
    mibLayout.append<Index>(N);
    CPUMesh res = addIndexBuffer(mibLayout, miMesh.mib.indices.size(),
                                               miMesh.mib.indices.data());
    // right now res.va contains the multi-dimensional indices instead of the actual data.
    // To replace the multi-dimensional indices with the actual data we do:
    res.va = applyMultiIndex(res.va.size(),
                             reinterpret_cast<std::array<Index, N>*>(res.va.data()),
                             miMesh.vas);
    return res;
}
*/
// TODO: change return type to CPUMesh
CPUVertexArray readOBJtest(std::string filepath);
//{
//    // ...
//    CPUMultiIndexMesh mim(...);
//    // ...
//    return unifyIndexBuffer(mim);
//}

#endif // IMPORT_MESH_H
