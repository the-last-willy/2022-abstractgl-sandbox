#pragma once

#include "vector.hpp"

namespace tlw {

template<std::size_t Size, typename Type> constexpr
auto operator+(const Vector<Size, Type>& lv, const Vector<Size, Type>& rv) {
    auto v = Vector<Size, Type>();
    for(std::size_t i = 0; i < Size; ++i) {
        v.at(i) = lv.at(i) + rv.at(i);
    }
    return v;
}

}
