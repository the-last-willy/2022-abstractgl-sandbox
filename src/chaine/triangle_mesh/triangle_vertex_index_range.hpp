#pragma once

#include "topology.hpp"
#include "triangle_index.hpp"

namespace chaine::triangle_mesh {

struct TriangleVertexIndexIterator {
    const Topology& topology;
    TriangleIndex triangle;
};

struct TriangleVertexIndexSentinel {

};

struct TriangleVertexIndexRange {
    TriangleVertexIndexIterator iterator;
};

inline
auto triangle_vertex_indices(const Topology& t) {
    return TriangleVertexIndexRange({
        TriangleVertexIndexIterator()});
}

}
