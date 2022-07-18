#pragma once

#include <cstdlib>

struct MeshId {
    std::size_t value = 0;

    MeshId() = default;

    explicit
    MeshId(std::size_t value)
        : value(value)
    {}

    operator std::size_t() const noexcept {
        return value;
    }
};
