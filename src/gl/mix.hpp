#pragma once

namespace tlw {
namespace gl {

template<typename Type> constexpr
auto mix(Type x, Type y, Type a) {
    return (1 - a) * x + a * y;
}

}}
