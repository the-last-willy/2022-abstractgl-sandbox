#pragma once

#include "gl/vector/vector.hpp"

namespace tlw {

template<std::size_t Size, typename Type> constexpr
bool operator==(
    const Vector<Size, Type>& lv,
    const Vector<Size, Type>& rv)
{
    auto b = true;
    for(std::size_t i = 0; i < Size; ++i) {
        b = b && (lv.at(i) == rv.at(i));
    }
    return b;
}

}
