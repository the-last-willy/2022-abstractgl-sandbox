#pragma once

#include "triangle_mesh_vertex_index.hpp"

#include <array>

namespace chaine::triangle_mesh {

struct TriangleTopology {
    std::array<VertexIndex> vertices = {};
};

}
