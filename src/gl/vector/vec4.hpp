#pragma once

#include "vector.hpp"
#include "xyzw.hpp"

namespace tlw {

constexpr
auto vec4(float x, float y, float z, float w) {
    auto v = Vector<4, float>();
    v.elements[0] = x;
    v.elements[1] = y;
    v.elements[2] = z;
    v.elements[3] = w;
    return v;
}

template<typename Type> constexpr
auto vec4(const Vector<3, Type>& v, float w) {
    return vec4(x(v), y(v), z(v), w);
}

}

