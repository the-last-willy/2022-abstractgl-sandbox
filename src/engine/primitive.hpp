#pragma once

#include "accessor.hpp"
#include "material.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace eng {

struct Primitive {
    // SHOULDN'T BE IN HERE.
    agl::VertexArray vertex_array = {};

    std::map<std::string, eng::Accessor> attributes = {};

    std::shared_ptr<Material> material = {};

    eng::Accessor indices = {};

    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    agl::DrawType draw_type;
    std::size_t offset = 0;
    
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
};

inline
void bind(const Primitive& p) {
    bind(p.vertex_array);
    if(p.material) {
        bind(*p.material);
    }
}

inline
void unbind(const Primitive& p) {
    if(p.material) {
        unbind(*p.material);
    }
} 

inline
void bind(Primitive& p, const Material& m) {
    for(int i = 0; i < agl::active_attributes(m.program.program); ++i) {
        auto aa = agl::active_attrib(m.program.program, agl::AttributeIndex(i));
        auto bi = agl::BindingIndex<GLuint>(i);
        auto ai = attribute_location(m.program.program, aa.name.c_str());
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
                accessor.buffer->opengl,
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
    } else {
        agl::draw_arrays(
            p.draw_mode,
            agl::Offset<GLint>(static_cast<GLint>(p.offset)),
            p.primitive_count);
    }
}

inline
void render(const Primitive& p, agl::VertexArray va) {
    auto b = agl::element_array_buffer_binding(va);
    if(agl::element_array_buffer_binding(va)) {
        agl::draw_elements(
            p.draw_mode,
            p.primitive_count,
            p.draw_type,
            agl::Offset<GLintptr>(p.offset));
    } else {
        agl::draw_arrays(
            p.draw_mode,
            agl::Offset<GLint>(static_cast<GLint>(p.offset)),
            p.primitive_count);
    }
}

}
