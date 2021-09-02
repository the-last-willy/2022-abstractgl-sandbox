#pragma once
#include <array>

struct Face {
    std::array<std::size_t, 3> vertices;
    std::array<std::size_t, 3> adjacents;
};
