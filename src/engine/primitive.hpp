#pragma once

#include "accessor.hpp"
#include "material.hpp"

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
    bind(p.material);
    bind(p.vertex_array);
}

inline
void unbind(const Primitive& p) {
    unbind(p.material);
}

inline
void render(const Primitive& p) {
    if(agl::element_array_buffer_binding(p.vertex_array)) {
        agl::draw_elements(
            p.draw_mode,
            p.primitive_count,
            p.draw_type,
            agl::Offset<GLintptr>(p.offset));
    }
}

}
