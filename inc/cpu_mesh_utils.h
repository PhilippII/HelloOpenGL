#ifndef CPU_MESH_UTILS_H
#define CPU_MESH_UTILS_H

#include "cpu_mesh_structs.h"

#include <limits>
#include <map>
#include <algorithm> // for std::equal(), std::copy()
#include <iterator> // for std::back_inserter()


template <typename Index>
CPUMesh<Index> addIndexBuffer(VertexBufferLayout layout,
                              Index count,
                              const GLbyte* data,
                              const GLbyte* restartVertex = nullptr,
                              Index primitiveRestartIndex = std::numeric_limits<Index>::max()) {
    // TODO:
    // - make res.ib.primitiveRestartIndex a std::optional<Index> and
    //      remove boolean res.ib.primitiveRestart?
    // - pass restartVertex as std::optional<std::vector<GLbyte>> ?
    // - make vertex and vertex_to_index's mapped_type a kind of a vector_view in order to avoid overhead
    //       of some unnecessary copy operations
    // - try a version of this function that casts function argument data
    //      to std::array<GLbyte, Stride>*
    //      -> advantage: avoids unnecessary copy to temp. variable vertex
    //              (we can just use data[i_in] directly instead of vertex)
    //              (but still does not avoid unnecessary copy to the map vertex_to_index)
    //      -> advantage: avoids complicated index calculation (i_in * stride) and ((i_in+1) * stride)
    //      -> disadvantage: needs to take Stride as template argument, because it must be constexpr
    //          -> add check: myAssert(Stride == layout.getStride())
    CPUMesh<Index> res;
    res.va.layout = layout;
    res.ib.primitiveRestart = (restartVertex != nullptr);
    res.ib.primitiveRestartIndex = primitiveRestartIndex;
    VertexBufferLayout::stride_type stride = layout.getStride();
    std::map<std::vector<GLbyte>, Index> vertex_to_index;
    for (Index i_in = 0; i_in < count; ++i_in) {
        myAssert(vertex_to_index.size() == res.va.data.size() / stride);
        Index i_out;
        std::vector<GLbyte> vertex(data+(i_in * stride), data+((i_in+1) * stride));
        if (restartVertex != nullptr && std::equal(vertex.begin(), vertex.end(), restartVertex)) {
            i_out = primitiveRestartIndex;
        } else {
            auto search = vertex_to_index.find(vertex);
            if (search != vertex_to_index.end()) {
                i_out = search->second;
            } else {
                i_out = static_cast<Index>(vertex_to_index.size()); // index, where the new vertex
                                                                    // will be added to the output vertex array
                std::copy(vertex.begin(), vertex.end(),
                          std::back_inserter(res.va.data));
                vertex_to_index.insert({vertex, i_out});
            }
        }
        res.ib.indices.push_back(i_out);
    }
    debugDo(std::cout << "removed " << (count - vertex_to_index.size()) << " doubles\n");
    debugDo(std::cout << vertex_to_index.size() << " vertices remaining\n");
    return res;
}

template <typename Index>
CPUMesh<Index> addIndexBuffer(const CPUVertexArray& va) {
    return addIndexBuffer<Index>(va.layout,
                                 static_cast<Index>(va.data.size() / va.layout.getStride()),
                                 va.data.data());
}

// assumes that there are no primitiveRestartIndices anymore in multiIndices
template <typename Index, int N>
CPUVertexArray applyMultiIndex(std::size_t count, // std::vector<...>::size_type does not work for some reason
                               const std::array<Index, N>* multiIndices,
                               const std::array<CPUVertexArray, N>& vas) {
    CPUVertexArray res;
    for (std::size_t i_vert = 0; i_vert < count; ++i_vert) {
        // TODO: i_buff and N should be unsigned int ?
        for (int i_va = 0; i_va < N; ++i_va) {
            VertexBufferLayout::stride_type stride_va = vas[i_va].layout.getStride();
            Index index = multiIndices[i_vert][i_va];
            std::copy(vas[i_va].data.data() +  index    * stride_va,
                      vas[i_va].data.data() + (index+1) * stride_va,
                      std::back_inserter(res.data));
        }
    }

    for (const auto& va : vas) {
        res.layout += va.layout;
    }

    return res;
}

template<typename Index, int N>
CPUVertexArray applyMultiIndex(const CPUMultiIndexMesh<Index, N>& miMesh) {
    myAssert(miMesh.mib.primitiveRestart == false);
    return applyMultiIndex<Index, N>(miMesh.mib.indices.size(),
                                     miMesh.mib.indices.data(),
                                     miMesh.vas);
}

template <typename Index, int N>
CPUMesh<Index> unifyIndexBuffer(const CPUMultiIndexMesh<Index, N>& miMesh,
                                Index restartIndex = std::numeric_limits<Index>::max()) {
    VertexBufferLayout mibLayout;
    mibLayout.append<Index>(N);
    CPUMesh<Index> res = addIndexBuffer<Index>(mibLayout,
                                               static_cast<Index>(miMesh.mib.indices.size()),
                                               reinterpret_cast<const GLbyte*>(miMesh.mib.indices.data()),
                                               (miMesh.mib.primitiveRestart) ?
                                                   reinterpret_cast<const GLbyte*>(miMesh.mib.primitiveRestartMultiIndex.data())
                                                   : nullptr,
                                               restartIndex);
    res.ib.primitiveType = miMesh.mib.primitiveType;
    // right now res.va contains the multi-dimensional indices instead of the actual data.
    // To replace the multi-dimensional indices with the actual data we do:
    res.va = applyMultiIndex<Index, N>(res.va.data.size() / sizeof(std::array<Index, N>),
                                       reinterpret_cast<const std::array<Index, N>*>(res.va.data.data()),
                                       miMesh.vas);
    return res;
}

template <typename Index>
CPUIndexBuffer<Index> applyTriangleFan(const CPUIndexBuffer<Index>& ib) {
    using ib_count_t = typename std::vector<Index>::size_type;
    CPUIndexBuffer<Index> res = {std::vector<Index>{},
                          GL_TRIANGLES,
                          false,
                          std::numeric_limits<Index>::max()};
    myAssert(ib.primitiveType == GL_TRIANGLE_FAN);
    myAssert(ib.primitiveRestart);

    ib_count_t i = 0;
    while (i < ib.indices.size()) {
        if (i + 2 >= ib.indices.size()) {
            std::cerr << "warning polygon with less than 3 vertices detected.\n";
            continue;
        }
        Index baseVert = ib.indices[i];
        i += 2;
        while (i < ib.indices.size() && ib.indices[i] != ib.primitiveRestartIndex) {
            res.indices.push_back(baseVert);
            res.indices.push_back(ib.indices[i-1]);
            res.indices.push_back(ib.indices[i]);
            ++i;
        }
        if (i < ib.indices.size()) { // restart index encountered
            ++i;
        }
    }
    return res;
}


#endif // CPU_MESH_UTILS_H
