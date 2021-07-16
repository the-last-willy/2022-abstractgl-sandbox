#pragma once

#include "vector.hpp"

#include <ostream>

namespace tlw {

template<std::size_t Size, typename Type>
auto& operator<<(std::ostream& os, const Vector<Size, Type>& v) {
    os << "(";
    if constexpr(Size > 0) {
        os << v.elements[0];
        for(std::size_t i = 1; i < Size; ++i) {
            os << ", " << v.elements[i];
        }
    }
    return os << ")";
}

}
