#pragma once

#include "agl/raw/qualifier/binding_index.hpp"
#include "agl/raw/qualifier/buffer.hpp"
#include "agl/raw/qualifier/offset.hpp"
#include "agl/raw/qualifier/stride.hpp"
#include "agl/raw/qualifier/vertex_array.hpp"

namespace agl {

inline
void vertex_array_vertex_buffer(
    VertexArray va,
    BindingIndex<GLuint> bi,
    Buffer b,
    Offset<GLintptr> o,
    Stride<GLsizei> s)
{
    glVertexArrayVertexBuffer(
        static_cast<GLuint>(va),
        static_cast<GLuint>(bi),
        static_cast<GLuint>(b),
        static_cast<GLintptr>(o),
        static_cast<GLsizei>(s));
}

inline
void vertex_buffer(
    VertexArray va,
    BindingIndex<GLuint> bi,
    Buffer b,
    Offset<GLintptr> o,
    Stride<GLsizei> s)
{
    vertex_array_vertex_buffer(va, bi, b, o, s);
}

}
