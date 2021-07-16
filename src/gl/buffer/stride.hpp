#pragma once

#include "buffer.hpp"

#include "gl/qualifier/stride.hpp"

namespace tlw {
namespace gl {

template<typename Traits>
auto stride(const Buffer<Traits>&) {
    return Stride<GLsizei>(
        sizeof(Buffer<Traits>::value_type));
}

}}
