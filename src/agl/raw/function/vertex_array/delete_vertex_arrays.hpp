#pragma once

#include "agl/raw/qualifier/buffer.hpp"

#include <span>

namespace agl {

inline
void delete_vertex_arrays(GLsizei n, VertexArray* vas) {
    glDeleteVertexArrays(
        n,
        reinterpret_cast<GLuint*>(vas));
}

inline
void delete_(std::span<VertexArray> s) {
    delete_vertex_arrays(
        static_cast<GLsizei>(size(s)),
        data(s));
}

inline
void detete_(VertexArray va) {
    delete_({&va, 1});
}

}
