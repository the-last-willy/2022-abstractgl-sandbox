#pragma once
#include <array>
#include <agl/all.hpp>

namespace chaine {

struct Face {
    agl::Uvec3 vertices;
    std::array<std::size_t, 3> adjacents;
};

}
