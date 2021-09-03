#pragma once

#include "triangle_topology.hpp"

#include <vector>

namespace chaine::triangle_mesh {

struct Topology {
    std::vector<TriangleTopology> triangles = {};
};

}
