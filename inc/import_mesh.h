#ifndef IMPORT_MESH_H
#define IMPORT_MESH_H

#include <string>
#include <vector>
#include <GL/glew.h>

#include "VertexBufferLayout.h"

template <typename Index>
struct CPUIndexBuffer {
    std::vector<Index> indices;
};
/*
template <typename Index, int N>
struct CPUMultiIndexBuffer {
    std::vector<std::array<Index, N>> indices;
};
*/
struct CPUVertexArray {
    VertexBufferLayout layout;
    std::vector<GLbyte> data;
};

template <typename Index>
struct CPUMesh {
    CPUIndexBuffer<Index> ib;
    CPUVertexArray va;
};
/*
template <typename Index, int N>
struct CPUMultiIndexMesh {
    CPUMultiIndexBuffer<Index, N> mib;
    std::array<CPUVertexArray, N> vas;
};
*/
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
    return addIndexBuffer<Index>(va.layout, va.data.size(), va.data.data());
}
/*
template <typename Index, int N>
CPUVertexArray applyMultiIndex(int count, const std::array<Index, N>* multiIndices, const std::array<CPUVertexArray>& vas);

CPUVertexArray applyMultiIndex(const CPUMultiIndexMesh& miMesh) {
    return applyMultiIndex(miMesh.mib.indices.size(),
                           miMesh.mib.indices.data(),
                           miMesh.vas);
}

template <typename Index, int N>
CPUMesh unifyIndexBuffer(CPUMultiIndexMesh miMesh) {
    VertexBufferLayout mibLayout;
    mibLayout.append<Index>(N);
    CPUMesh indexedMultiIndex = addIndexBuffer(mibLayout, miMesh.mib.indices.size(),
                                               miMesh.mib.indices.data());
    CPUVertexArray va = applyMultiIndex(indexedMultiIndex.va.size(),
                                        reinterpret_cast<std::array<Index, N>*>(indexedMultiIndex.va.data()),
                                        miMesh.vas
                                    );
    return CPUMesh{indexedMultiIndex.ib, va};
}
*/
// TODO: change return type to CPUMesh
void readOBJtest(std::string filepath);
//{
//    // ...
//    CPUMultiIndexMesh mim(...);
//    // ...
//    return unifyIndexBuffer(mim);
//}

#endif // IMPORT_MESH_H
