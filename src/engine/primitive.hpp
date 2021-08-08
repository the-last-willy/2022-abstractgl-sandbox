#pragma once

#include "material.hpp"

namespace eng {

struct Primitive {
    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    agl::DrawType draw_type;
    std::size_t offset = 0;
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
    
    Material* material = nullptr;

    agl::VertexArray vertex_array = {};
};

inline
void bind(const Primitive& p) {
    bind(*p.material);
    bind(p.vertex_array);
}

inline
void unbind(const Primitive& p) {
    unbind(*p.material);
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
