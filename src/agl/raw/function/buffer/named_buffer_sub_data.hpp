#pragma once

#include "agl/raw/qualifier/buffer.hpp"

#include <cstddef>
#include <span>

namespace agl {

inline
void named_buffer_sub_data(Buffer b, GLintptr offset, GLsizeiptr size, const std::byte* data) {
    glNamedBufferSubData(
        static_cast<GLuint>(b),
        offset,
        size,
        data);
}

inline
void sub_data(Buffer b, GLintptr offset, std::span<const std::byte> s) {
    named_buffer_sub_data(
        b,
        offset,
        static_cast<GLsizeiptr>(size(s)),
        data(s));
}

template<typename Type>
void sub_data(Buffer b, GLintptr offset, std::span<const Type> s) {
    sub_data(b, offset, s.as_bytes());
}

}
