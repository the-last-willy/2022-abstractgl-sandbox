#pragma once

#include "agl/raw/qualifier/buffer.hpp"

#include <cstddef>

namespace agl {

inline
std::byte* map_named_buffer(Buffer b, GLbitfield access) {
    auto m = glMapNamedBuffer(
        static_cast<GLuint>(b),
        access);
    return reinterpret_cast<std::byte*>(m);
}

template<typename Type>
Type* map(Buffer b, GLbitfield access) {
    auto m = map_named_buffer(b, access);
    return reinterpret_cast<Type*>(m);
}

}
