#pragma once

#include "gl/vector/vector.hpp"

namespace tlw {

template<std::size_t Size, typename Type> constexpr
auto& operator-=(Vector<Size, Type> lv, const Vector<Size, Type>& rv) {
    for(std::size_t i = 0; i < Size; ++i) {
        lv.at(i) -= rv.at(i);
    }
    return lv;
}

}
