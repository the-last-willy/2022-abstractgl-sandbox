#pragma once

#include "agl/raw/qualifier/count.hpp"
#include "agl/raw/qualifier/draw_mode.hpp"
#include "agl/raw/qualifier/offset.hpp"

namespace agl {

inline
void draw_arrays(DrawMode dm, Offset<GLint> o, Count<GLsizei> c) {
    glDrawArrays(
        static_cast<GLenum>(dm),
        static_cast<GLint>(o),
        static_cast<GLsizei>(c));
}

}
