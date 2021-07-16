#pragma once

#include "agl/glsl/vec/vec.hpp"

#include <array>

namespace agl {

template<typename Type, std::size_t ColumnCount, std::size_t RowCount>
class Mat {
public:


private:
    std::array<Vec<Type, RowCount>, ColumnCount> colums;
};



}
