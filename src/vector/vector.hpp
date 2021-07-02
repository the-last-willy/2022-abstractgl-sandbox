#pragma once

#include <array>

#include "glad/glad.h"

namespace tlw {

template<std::size_t Size, typename Type>
struct Vector {

    constexpr
    auto& at(std::size_t i) const {
        return elements.at(i);
    }

    constexpr
    auto& at(std::size_t i) {
        return elements.at(i);
    }

    constexpr
    auto size() const {
        return Size;
    }

    std::array<Type, Size> elements = {};
};

using Vec3 = Vector<3, float>;

constexpr
auto vec3(float x, float y, float z) {
    auto v = Vector<3, float>();
    v.elements[0] = x;
    v.elements[1] = y;
    v.elements[2] = z;
    return v;
}

}
