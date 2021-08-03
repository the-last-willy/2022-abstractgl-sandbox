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

inline
void configure(Object& o) {
    auto geometry = *o.geometry;
    auto program = o.material->program;

    auto& va = o.vertex_array;

    if(geometry.indexes) {
        element_buffer(va, *geometry.indexes);
    }
    for(int i = 0; i < agl::active_attributes(program); ++i) {
        auto ai = agl::AttributeIndex(i);
        auto aa = agl::active_attrib(program, ai);
        auto attribute = geometry.attributes.at(aa.name);

        auto bi = agl::BindingIndex<GLuint>(i);

        attribute_binding(va, ai, bi);
        attribute_format(va, ai,
            attribute.size, attribute.type);
        vertex_buffer(va, bi,
        attribute.buffer, attribute.offset, attribute.stride);
        enable(va, ai);
    }
}
