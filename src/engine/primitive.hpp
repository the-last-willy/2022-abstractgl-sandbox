#pragma once

#include "accessor.hpp"
#include "material.hpp"

#include <stdexcept>
#include <string>

namespace eng {

struct Primitive {
    agl::VertexArray vertex_array = {};

    std::map<std::string, eng::Accessor> attributes = {};

    Material material = {};

    // Indices.

    eng::Accessor indices = {};

    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    agl::DrawType draw_type;
    std::size_t offset = 0;
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
};

inline
void bind(const Primitive& p) {
    bind(p.vertex_array);
    bind(p.material);
}

inline
void unbind(const Primitive& p) {
    unbind(p.material);
} 

inline
void bind(Primitive& p, const Material& m) {
    for(int i = 0; i < agl::active_attributes(m.program.program); ++i) {
        auto ai = agl::AttributeIndex(i);
        auto aa = agl::active_attrib(m.program.program, ai);
        auto bi = agl::BindingIndex<GLuint>(i);
        attribute_binding(p.vertex_array, ai, bi);

        auto it = p.attributes.find(aa.name);
        if(it != end(p.attributes)) {
            auto& accessor = it->second;
            attribute_format(
                p.vertex_array, ai,
                accessor.component_count,
                accessor.component_type,
                accessor.normalized,
                agl::Offset<GLuint>(0));
            vertex_buffer(
                p.vertex_array, bi,
                accessor.buffer,
                agl::Offset<GLintptr>(accessor.buffer_view_byte_offset.value + accessor.byte_offset.value),
                accessor.buffer_view_byte_stride);
            enable(p.vertex_array, ai);
        } else {
            // std::cout << "Missing " << aa.name << std::endl;
            // throw std::runtime_error("Missing vertex attribute.");
        }
    }
    bind(p.vertex_array);
}

inline
void unbind(const Primitive& p, const Material& m) {
    unbind(agl::vertex_array_tag);
    for(int i = 0; i < agl::active_attributes(m.program.program); ++i) {
        auto ai = agl::AttributeIndex(i);
        disable(p.vertex_array, ai);
    }
}

inline
void render(const Primitive& p) {
    auto b = agl::element_array_buffer_binding(p.vertex_array);
    if(agl::element_array_buffer_binding(p.vertex_array)) {
        agl::draw_elements(
            p.draw_mode,
            p.primitive_count,
            p.draw_type,
            agl::Offset<GLintptr>(p.offset));
    }
}

}
