#pragma once

#include "material.hpp"
#include "primitive.hpp"
#include "uniform.hpp"

#include <agl/all.hpp>

#include <deque>
#include <map>
#include <optional>
#include <set>
#include <vector>

namespace eng {

struct Database {
    Material default_material = {};

    // Textures.
    agl::Texture default_albedo_map = {};
    agl::Texture default_normal_map = {};

    agl::VertexArray empty_vertex_array = agl::vertex_array();
};

}
