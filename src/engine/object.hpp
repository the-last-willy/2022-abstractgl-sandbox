#pragma once

#include "geometry.hpp"
#include "material.hpp"
#include "uniform.hpp"

#include <agl/all.hpp>

#include <memory>

struct Object {
    std::shared_ptr<Geometry> geometry = {};
    std::shared_ptr<Material> material = {};

    std::map<std::string, std::unique_ptr<AnyUniform>> uniforms = {};
    
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

inline
void render(const Object& object) {
    auto& geometry = *object.geometry;
    auto& material = *object.material;
    for(auto& p : material.parameters) {
        p();
    }
    for(auto c : material.capabilities) {
        agl::enable(c);
    }
    auto program = material.program;
    agl::use(program);
    { // Uniforms.
        for(int idx = 0; idx < agl::active_uniforms(program); ++idx) {
            auto ui = agl::UniformIndex(idx);
            auto info = agl::active_uniform(
                program, agl::UniformIndex<GLuint>(ui));
            auto it = object.uniforms.find(info.name);
            if(it != end(object.uniforms)) {
                it->second->set(program, ui);
            }
        }
    }
    { // Draw.
        agl::bind(object.vertex_array);

        if(geometry.indexes) {
            agl::draw_elements(
                geometry.draw_mode,
                geometry.primitive_count,
                agl::DrawType::unsigned_int);
        } else {
            agl::draw_arrays(
                geometry.draw_mode,
                agl::Offset<GLint>(0),
                geometry.primitive_count);
        }
    }
    for(auto c : material.capabilities) {
        agl::disable(c);
    }
}
