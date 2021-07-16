#pragma once

#include "gl/vector/vector.hpp"

#include <cmath>

namespace tlw {

template<std::size_t Size, typename Type>
auto floor(const Vector<Size, Type>& v) {
    auto f = Vector<Size, Type>();
    for(std::size_t i = 0; i < Size; ++i) {
        f.at(i) = std::floor(v.at(i));
    }
    return f;
}

}
