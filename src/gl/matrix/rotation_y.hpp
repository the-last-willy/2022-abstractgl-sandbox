#pragma once

#include "matrix.hpp"

#include <cmath>

namespace tlw {

inline
auto rotation_y(float radians) {
    auto c = std::cos(radians);
    auto s = std::sin(radians);
    auto m = mat4_identity();
    // 2D rotation matrix.
    // | c | -s |
    // | s |  c |
    // 3D rotation matrix.
    // |  c | 0 | s | 0 |
    // |  0 | 1 | 0 | 0 |
    // | -s | 0 | c | 0 |
    // |  0 | 0 | 0 | 1 |
    m.at(0, 0) = c;
    m.at(0, 2) = -s;
    m.at(2, 0) = s;
    m.at(2, 2) = c;
    return m;
}

}
