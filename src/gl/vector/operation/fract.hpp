#pragma once

#include "floor.hpp"

#include "gl/vector/operator/subtraction.hpp"

#include <cmath>

namespace tlw {

template<std::size_t Size, typename Type>
auto fract(const Vector<Size, Type>& v) {
    return v - floor(v);
}

}