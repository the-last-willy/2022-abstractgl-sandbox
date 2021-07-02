#pragma once

#include "vertex_array.hpp"

namespace tlw {
namespace gl {

inline
void bind(const VertexArray& va) {
    glBindVertexArray(va);
}

inline
void unbind_vertex_array() {
    glBindVertexArray(0);
}

}}
