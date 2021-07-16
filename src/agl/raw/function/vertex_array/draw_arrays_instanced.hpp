#pragma once

#include "agl/raw/qualifier/count.hpp"
#include "agl/raw/qualifier/draw_mode.hpp"
#include "agl/raw/qualifier/offset.hpp"
#include "agl/raw/qualifier/instance_count.hpp"

namespace agl {

inline
void draw_arrays_instanced(
    DrawMode dm,
    Offset<GLint> o,
    Count<GLsizei> c,
    InstanceCount<GLsizei> ic) {
    glDrawArraysInstanced(
        static_cast<GLenum>(dm),
        static_cast<GLint>(o),
        static_cast<GLsizei>(c),
        static_cast<GLsizei>(ic));
}

inline
void draw_arrays(
    DrawMode dm,
    Offset<GLint> o,
    Count<GLsizei> c,
    InstanceCount<GLsizei> ic)
{
    draw_arrays_instanced(dm, o, c, ic);
}

}
