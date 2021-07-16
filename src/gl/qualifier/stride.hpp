#pragma once

namespace tlw {
namespace gl {

template<typename Type>
struct Stride {
    using value_type = Type;

    constexpr Stride() noexcept = default;

    constexpr explicit Stride(value_type t) noexcept
        : value(t)
    {}

    operator const value_type&() const noexcept {
        return value;
    }

    operator value_type&() noexcept {
        return value;
    }

    value_type value = 0;
};

template<typename Type>
Stride(Type) -> Stride<Type>;

}}
