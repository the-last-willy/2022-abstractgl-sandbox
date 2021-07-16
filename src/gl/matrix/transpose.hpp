#pragma once

#include "matrix.hpp"

namespace tlw {

template<std::size_t CC, std::size_t RC, typename Type> constexpr
auto transpose(const Matrix<CC, RC, Type>& m) {
    auto t = Matrix<RC, CC, Type>();
    for(std::size_t c = 0; c < CC; ++c)
    for(std::size_t r = 0; r < RC; ++r) {
        t(r, c) = m(c, r);
    }
    return t;
}

}
