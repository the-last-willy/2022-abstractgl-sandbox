#pragma once

#include "matrix.hpp"

#include "vector/all.hpp"

namespace tlw {

template<std::size_t I, std::size_t J, std::size_t K, typename Type> constexpr
auto operator*(const Matrix<I, J, Type>& lm , const Matrix<K, I, Type>& rm) {
    auto m = Matrix<K, J, Type>();
    for(std::size_t k = 0; k < K; ++k)
    for(std::size_t i = 0; i < I; ++i)
    for(std::size_t j = 0; j < J; ++j) {
        m.at(k, j) += lm.at(i, j) * rm.at(k, i);
    }
    return m;
}

template<std::size_t I, std::size_t J, typename Type> constexpr
auto operator*(const Matrix<I, J, Type>& lm , const Vector<I, Type>& rv) {
    auto r = Vector<J, Type>();
    for(std::size_t j = 0; j < J; ++j)
    for(std::size_t i = 0; i < I; ++i) {
        r.at(j) += lm.at(i, j) * rv.at(i);
    }
    return r;
}

}
