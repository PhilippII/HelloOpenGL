#ifndef IMPORT_MESH_H
#define IMPORT_MESH_H

#include <string>
#include <vector>
#include <GL/glew.h>
/*
template <typename Index>
struct CPUIndexBuffer {
    std::vector<Index> indices;
};

template <typename Index, int N>
struct CPUMultiIndexBuffer {
    std::vector<std::array<Index, N>> indices;
};

struct CPUVertexArray {
    Layout layout;
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

CPUMesh addIndexBuffer(const Layout& layout, GLuint* data, int count);

CPUMesh addIndexBuffer(const CPUVertexArray& va) {
    return addIndexBuffer(va.layout, va.data.data(), va.data.size());
}

template <typename Index, int N>
CPUVertexArray applyMultiIndex(const std::array<Index, N>* multiIndices, int count, const std::array<CPUVertexArray>& vas);

CPUVertexArray applyMultiIndex(CPUMultiIndexMesh miMesh) {
    return applyMultiIndex(miMesh.mib.indices.data(),
                           miMesh.mib.indices.size(),
                           miMesh.vas);
}

template <typename Index, typename N>
CPUMesh unifyIndexBuffer(CPUMultiIndexMesh miMesh) {
    Layout miLayout;
    miLayout.push_back<Index>(N);
    CPUMesh indexedMultiIndex = addIndexBuffer(miLayout, miMesh.mib.indices.data(), miMesh.mib.indices.size());
    CPUVertexArray va = applyMultiIndex(reinterpret_cast<std::array<Index, N>*>(indexedMultiIndex.va.data()),
                                        indexedMultiIndex.va.size(),
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
