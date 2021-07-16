#pragma once

#include <glad/glad.h>

#include <cstdlib>
#include <array>

namespace agl {

template<typename Type, std::size_t Length>
class Vec {
    std::array<Type, Length> elements;

public:
    constexpr Vec() = default;

    auto operator[](std::size_t i) const {
        return elements[i];
    }

    auto& operator[](std::size_t i) {
        return elements[i];
    }
};

}
