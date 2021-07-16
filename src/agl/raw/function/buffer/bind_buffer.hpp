#pragma once

#include "agl/raw/qualifier/buffer.hpp"
#include "agl/raw/qualifier/buffer_target.hpp"

namespace agl {

inline
void bind_buffer(BufferTarget t, Buffer b) {
    glBindBuffer(
        static_cast<GLenum>(t),
        static_cast<GLuint>(b));
}

inline
void bind(BufferTarget t, Buffer b) {
    bind_buffer(t, b);
}

}
