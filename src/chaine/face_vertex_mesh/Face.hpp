#pragma once
#include <vector>

struct Face {
    std::vector<std::size_t> vertices = {};
    std::vector<std::size_t> adjacents = {};
};