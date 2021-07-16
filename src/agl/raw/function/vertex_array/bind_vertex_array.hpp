#pragma once

#include "agl/raw/qualifier/vertex_array.hpp"

namespace agl {

inline
void bind_vertex_array(VertexArray va) {
    glBindVertexArray(static_cast<GLuint>(va));
}

inline
void bind(VertexArray va) {
    bind_vertex_array(va);
}

inline
void unbind_vertex_array() {
    glBindVertexArray(0);
}

}
