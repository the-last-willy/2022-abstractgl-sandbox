#pragma once

#include "length.hpp"

#include "gl/vector/operator/division.hpp"

#include <stdexcept>

namespace tlw {

template<std::size_t Size, typename Type>
auto normalized(const Vector<Size, Type>& v) {
    auto l = length(v);
    if(l == 0) {
        throw std::domain_error("Div by 0.");
    }
    return v / l;
} 

}
