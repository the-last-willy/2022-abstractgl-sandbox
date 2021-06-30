#pragma once

#include "matrix.hpp"

#include <cmath>

namespace tlw {

inline
auto rotation_y(float radians) {
    auto c = std::cos(radians);
    auto s = std::sin(radians);
    auto m = Matrix<4, 4, float>();
    // 2D rotation matrix.
    // | c | -s |
    // | s |  c |
    // 3D rotation matrix.
    // | c | 0 | -s | 0 |
    // | 0 | 1 |  0 | 0 |
    // | s | 0 |  c | 0 |
    // | 0 | 0 |  0 | 1 |
    m.at(0, 0) = c;
    m.at(0, 2) = s;
    m.at(1, 1) = 1.f;
    m.at(2, 0) = -s;
    m.at(2, 2) = c;
    m.at(3, 3) = 1.f;
    return m;
}

}
