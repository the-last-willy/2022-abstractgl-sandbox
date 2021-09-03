#pragma once

#include "vertex_index.hpp"

namespace chaine::triangle_mesh {

struct EdgeIndex {
    VertexIndex head, tail;
};

}
