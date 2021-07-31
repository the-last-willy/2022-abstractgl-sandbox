#pragma once

#include "geometry.hpp"
#include "material.hpp"

#include <agl/all.hpp>

#include <memory>

struct Object {
    std::shared_ptr<Geometry> geometry = {};
    std::shared_ptr<Material> material = {};

    std::map<std::string, std::any> uniforms = {};
    agl::VertexArray vertex_array = agl::vertex_array();
};
