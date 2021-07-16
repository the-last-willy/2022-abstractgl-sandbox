#pragma once

#include "agl/raw/qualifier/buffer.hpp"

#include <span>

namespace agl {

inline
void delete_buffers(GLsizei n, Buffer* buffers) {
    glDeleteBuffers(
        n,
        reinterpret_cast<GLuint*>(buffers));
}

inline
void delete_(std::span<Buffer> s) {
    delete_buffers(
        static_cast<GLsizei>(size(s)),
        data(s));
}

inline
void delete_(Buffer b) {
    delete_({&b, 1});
}

}
