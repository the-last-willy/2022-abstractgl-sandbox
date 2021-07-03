#pragma once

#include "vector.hpp"

namespace tlw {

template<std::size_t Size, typename Type> constexpr
auto operator-(const Vector<Size, Type>& v) {
    auto r = Vector<Size, Type>();
    for(std::size_t i = 0; i < Size; ++i) {
        r.at(i) = -v.at(i);
    }
    return r;
}

}
