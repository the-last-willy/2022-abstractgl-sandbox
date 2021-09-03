#pragma once

#include "vertex_index.hpp"

#include <array>

namespace chaine::triangle_mesh {

struct TriangleTopology {
    std::array<VertexIndex, 3> vertices = {};
};

}
