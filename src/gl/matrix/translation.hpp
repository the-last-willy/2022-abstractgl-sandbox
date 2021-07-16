#pragma once

#include "matrix.hpp"

#include "vector/vector.hpp"
#include "vector/xyzw.hpp"

namespace tlw {

constexpr
auto translation(float x, float y, float z) {
    // | 1 | 0 | 0 | x |
    // | 0 | 1 | 0 | y |
    // | 0 | 0 | 1 | z |
    // | 0 | 0 | 0 | 1 |
    auto m = Matrix<4, 4, float>();
    m.at(0, 0) = 1;
    m.at(1, 1) = 1;
    m.at(2, 2) = 1;
    m.at(3, 3) = 1;
    m.at(3, 0) = x;
    m.at(3, 1) = y;
    m.at(3, 2) = z;
    return m;
}

constexpr
auto translation(const Vector<3, float>& v) {
    return translation(x(v), y(v), z(v));
}

}
