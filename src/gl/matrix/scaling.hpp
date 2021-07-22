#pragma once

#include "agl/glsl/mat/mat4.hpp"

namespace tlw {


inline
auto scaling(float s) {
    auto m = Mat4();
    m.at(0, 0) = s;
    m.at(1, 1) = s;
    m.at(2, 2) = s;
    m.at(3, 3) = 1.f;
    return m;
}

}
