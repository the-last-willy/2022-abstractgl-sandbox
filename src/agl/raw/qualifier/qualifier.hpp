#pragma once

#include <glad/glad.h>

namespace agl {

template<typename Type>
struct Qualifier {
    constexpr Qualifier() = default;

    constexpr explicit Qualifier(Type value) noexcept
        : value(value)
    {}

    explicit operator const auto&() const noexcept {
        return value;
    }

    Type value;
};

}
