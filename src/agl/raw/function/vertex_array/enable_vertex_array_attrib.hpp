#pragma once

#include "agl/raw/qualifier/attribute_index.hpp"
#include "agl/raw/qualifier/vertex_array.hpp"

namespace agl {

inline
void enable_vertex_array_attrib(
    VertexArray va,
    AttributeIndex<GLuint> ai)
{
    glEnableVertexArrayAttrib(
        static_cast<GLuint>(va),
        static_cast<GLuint>(ai));
}

inline
void enable(VertexArray va, AttributeIndex<GLuint> ai) {
    enable_vertex_array_attrib(va, ai);
}

}
