#pragma once

#include <array>
#include <stdexcept>

#include <glad/glad.h>

namespace tlw {

template<std::size_t ColumnCount, std::size_t RowCount, typename Type>
class Matrix {
public:
    static_assert(ColumnCount > 0,
        "A matrix shoud have at least one column.");
    static_assert(RowCount > 0,
        "A matrix shoud have at least one row.");

    constexpr
    auto& at(std::size_t col, std::size_t row) const {
        if(col >= ColumnCount) {
            throw std::out_of_range(
                "Matrix: Invalid column index.");
        }
        if(row >= RowCount) {
            throw std::out_of_range(
                "Matrix: Invalid row index.");
        }
        return elements[column_major_index(col, row)];
    }

    constexpr
    auto& at(std::size_t col, std::size_t row) {
        if(col >= ColumnCount) {
            throw std::out_of_range(
                "Matrix: Invalid column index.");
        }
        if(row >= RowCount) {
            throw std::out_of_range(
                "Matrix: Invalid row index.");
        }
        return elements[column_major_index(col, row)];
    }

    constexpr
    auto column_count() const {
        return ColumnCount;
    }

    constexpr
    auto row_count() const {
        return RowCount;
    }

    constexpr
    std::size_t column_major_index(
        std::size_t col, std::size_t row) const
    {
        return col * row_count() + row;
    }

    std::array<Type, ColumnCount * RowCount> elements = {}; 
};

}
