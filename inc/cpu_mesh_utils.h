#ifndef CPU_MESH_UTILS_H
#define CPU_MESH_UTILS_H

#include "cpu_mesh_structs.h"

#include <limits>
#include <map>
#include <algorithm> // for std::equal(), std::copy()
#include <iterator> // for std::back_inserter()
#include "gsl/gsl" // or "gsl/gsl" ?


// TODO: maybe SpanDeepCompareLess out of public namespace?
template <typename Elem>
class SpanDeepCompareLess {
public:
    bool operator()(gsl::span<Elem> a, gsl::span<Elem> b) const {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }
    // using this comparison function map will consider two spans equivalent
    // if they point to arrays with the same content, even if the
    // actual memory adresses may be different
};

template <typename Index>
CPUMesh<Index> addIndexBuffer(const CPUVertexArray& va,
                              std::optional<gsl::span<const GLbyte>> restartVertex = {},
                              Index primitiveRestartIndex = std::numeric_limits<Index>::max()) {
    VertexBufferLayout::stride_type stride = va.layout.getStride();
    ASSERT(va.data.size() % stride == 0);
    const Index vertCount_in = static_cast<Index>(va.data.size() / stride);

    CPUMesh<Index> res;
    res.va.layout = va.layout;
    ASSERT(!restartVertex || restartVertex->size() == static_cast<std::size_t>(stride));
    res.ib.primitiveRestartIndex = (restartVertex) ? std::optional<Index>{primitiveRestartIndex} : std::nullopt;

    SpanDeepCompareLess<const GLbyte> comp;
    std::map<gsl::span<const GLbyte>, Index, decltype(comp)> vertex_to_i_out(comp);

    for (Index i_in = 0; i_in < vertCount_in; ++i_in) {
        Index i_out = 0;
        ASSERT(vertex_to_i_out.size() == res.va.data.size() / stride);
        gsl::span<const GLbyte> vertex(va.data.data() + i_in * stride, stride);
        if (restartVertex && std::equal(vertex.begin(), vertex.end(), restartVertex->begin())) {
            i_out = primitiveRestartIndex;
        } else {
             auto search = vertex_to_i_out.find(vertex);
             if (search != vertex_to_i_out.end()) {
                 i_out = search->second;
             } else {
                 i_out = static_cast<Index>(vertex_to_i_out.size());
                 std::copy(vertex.begin(), vertex.end(), std::back_inserter(res.va.data));
                 vertex_to_i_out.insert({vertex, i_out});
             }
        }
        res.ib.indices.push_back(i_out);
    }
    DEBUG_DO(std::cout << "removed " << (vertCount_in - vertex_to_i_out.size()) << " duplicate vertices and/or restartVertices\n");
    DEBUG_DO(std::cout << vertex_to_i_out.size() << " vertices remaining in vertex array\n");

    return res;
}


template <typename Index, int N>
CPUMesh<Index> unifyIndexBuffer(const CPUMultiIndexMesh<Index, N>& miMesh,
                                Index restartIndex = std::numeric_limits<Index>::max()) {
    // 1. remove duplicates from multidimensional index buffer and add a single
    //      dimensional index buffer that references the multidimensional index buffer:
    CPUMesh<Index> res;
    res.ib.primitiveType = miMesh.mib.primitiveType;
    res.ib.primitiveRestartIndex = restartIndex;
    std::vector<std::array<Index, N>> mib;
    std::map<std::array<Index, N>, Index> multiIndex_to_i_out;
    for (std::size_t i_in = 0; i_in < miMesh.mib.indices.size(); ++i_in) {
        Index i_out = 0;
        const std::array<Index, N>& multiIndex = miMesh.mib.indices[i_in];
        if (multiIndex == miMesh.mib.primitiveRestartMultiIndex) {
            i_out = restartIndex;
        } else {
            auto search = multiIndex_to_i_out.find(multiIndex);
            if (search != multiIndex_to_i_out.end()) {
                i_out = search->second;
            } else {
                i_out = mib.size();
                mib.push_back(multiIndex);
                multiIndex_to_i_out.insert({multiIndex, i_out});
            }
        }
        res.ib.indices.push_back(i_out);
    }

    // 2. replace mib with actual data:
    for (auto& multiIndex : mib) {
        for (int i_va = 0; i_va < N; ++i_va) {
            const CPUVertexArray& va = miMesh.vas[i_va];
            auto stride = va.layout.getStride();
            std::copy(va.data.data() + multiIndex[i_va] * stride,
                      va.data.data() + multiIndex[i_va] * stride + stride,
                      std::back_inserter(res.va.data));
        }
    }
    for (int i_va = 0; i_va < N; ++i_va) {
        res.va.layout += miMesh.vas[i_va].layout;
    }

    return res;
}

template <typename Index>
CPUIndexBuffer<Index> applyTriangleFan(const CPUIndexBuffer<Index>& ib) {
    using ib_count_t = typename std::vector<Index>::size_type;
    CPUIndexBuffer<Index> res = {std::vector<Index>{},
                                 GL_TRIANGLES,
                                 {} // disable primitiveRestartIndex in result
                                };
    ASSERT(ib.primitiveType == GL_TRIANGLE_FAN);
    ASSERT(ib.primitiveRestartIndex);

    ib_count_t i = 0;
    while (i < ib.indices.size()) {
        if (i + 2 >= ib.indices.size()) {
            std::cerr << "warning polygon with less than 3 vertices detected.\n";
            continue;
        }
        Index baseVert = ib.indices[i];
        i += 2;
        while (i < ib.indices.size() && ib.indices[i] != *ib.primitiveRestartIndex) {
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
