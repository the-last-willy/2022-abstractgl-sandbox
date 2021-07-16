#pragma once

#include "agl/raw/qualifier/vertex_array.hpp"

#include <span>

namespace agl {

inline
void create_vertex_arrays(GLsizei n, VertexArray* vas) {
    glCreateVertexArrays(
        n,
        reinterpret_cast<GLuint*>(vas));
}

inline
void create(std::span<VertexArray> s) {
    create_vertex_arrays(
        static_cast<GLsizei>(size(s)),
        data(s));
}

inline
VertexArray vertex_array() {
    auto va = VertexArray();
    create({&va, 1});
    return va;
}

}
