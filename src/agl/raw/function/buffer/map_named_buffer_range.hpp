#pragma once

#include "agl/raw/qualifier/buffer.hpp"

#include <cstddef>

namespace agl {

inline
std::byte* map_named_buffer_range(
    Buffer b,
    GLintptr offset,
    GLsizeiptr length,
    GLbitfield access)
{
    auto m = glMapNamedBufferRange(
        static_cast<GLuint>(b),
        offset,
        length,
        access);
    return reinterpret_cast<std::byte*>(m);
}

template<typename Type>
Type* map(
    Buffer b,
    GLintptr offset,
    GLsizeiptr count,
    GLbitfield access)
{
    auto m = map_named_buffer_range(
        b,
        offset,
        count * sizeof(Type),
        access);
    return reinterpret_cast<Type*>(m);
}

}
