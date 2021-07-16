#pragma once

#include "gl/vector/vector.hpp"

#include <cmath>

namespace tlw {

template<std::size_t Size, typename Type>
auto length(const Vector<Size, Type>& v) {
    auto l = Type{0};
    for(auto e : v.elements) {
        l += e * e;
    }
    return std::sqrt(l);
} 

}
