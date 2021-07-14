#pragma once

#include "buffer.hpp"

#include "gl/stride.hpp"

namespace tlw {
namespace gl {

template<typename Traits>
auto stride(const Buffer<Traits>&) {
    return Stride(sizeof(Buffer<Traits>::value_type));
}

}}
