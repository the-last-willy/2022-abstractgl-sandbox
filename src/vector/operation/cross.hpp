#pragma once

#include "vector/vector.hpp"

namespace tlw {

template<typename Type> constexpr
auto cross(const Vector<3, Type>& lv, const Vector<3, Type>& rv) {
    return Vector<3, Type>{
        lv.at(1) * rv.at(2) - lv.at(2) * rv.at(1),
        lv.at(2) * rv.at(0) - lv.at(0) * rv.at(2),
        lv.at(0) * rv.at(1) - lv.at(1) * rv.at(0)};
}

}
