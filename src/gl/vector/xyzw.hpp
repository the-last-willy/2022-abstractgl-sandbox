#pragma once

#include "vector.hpp"

namespace tlw {

template<std::size_t Size, typename Type> constexpr
auto& x(const Vector<Size, Type>& v) {
    static_assert(Size >= 1,
        "Vector should have at least 1 element.");
    return v.elements[0];
}

template<std::size_t Size, typename Type> constexpr
auto& x(Vector<Size, Type>& v) {
    static_assert(Size >= 1,
        "Vector should have at least 1 element.");
    return v.elements[0];
}

template<std::size_t Size, typename Type> constexpr
auto& y(const Vector<Size, Type>& v) {
    static_assert(Size >= 2,
        "Vector should have at least 2 elements.");
    return v.elements[1];
}

template<std::size_t Size, typename Type> constexpr
auto& y(Vector<Size, Type>& v) {
    static_assert(Size >= 2,
        "Vector should have at least 2 elements.");
    return v.elements[1];
}

template<std::size_t Size, typename Type> constexpr
auto& z(const Vector<Size, Type>& v) {
    static_assert(Size >= 3,
        "Vector should have at least 3 elements.");
    return v.elements[2];
}

template<std::size_t Size, typename Type> constexpr
auto& z(Vector<Size, Type>& v) {
    static_assert(Size >= 3,
        "Vector should have at least 3 elements.");
    return v.elements[2];
}

template<std::size_t Size, typename Type> constexpr
auto& w(const Vector<Size, Type>& v) {
    static_assert(Size >= 4,
        "Vector should have at least 4 elements.");
    return v.elements[3];
}

template<std::size_t Size, typename Type> constexpr
auto& w(Vector<Size, Type>& v) {
    static_assert(Size >= 4,
        "Vector should have at least 4 elements.");
    return v.elements[3];
}

template<std::size_t Size, typename Type> constexpr
auto xyz(const Vector<Size, Type>& v) {
    static_assert(Size >= 3,
        "Vector should have at least 3 elements.");
    return vec3(x(v), y(v), z(v));
}

}
