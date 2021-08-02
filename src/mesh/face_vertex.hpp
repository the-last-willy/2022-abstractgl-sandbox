#pragma once

#include <array>
#include <vector>

namespace mesh {

struct Triangle {
    std::array<unsigned, 3> vertices = {};

    constexpr
    auto operator[](std::size_t i) const {
        return vertices[i];
    }
};

template<typename Vertex>
struct FaceVertex {
    std::vector<Triangle> triangles = {};
    std::vector<Vertex> vertices = {};
};

}
