#pragma once

#include "object.hpp"

#include <agl/all.hpp>

#include <typeindex>

template<typename... Types>
struct TypeList {};

using rtti_uniform_map = std::map<
    std::type_index,
    std::function<void(
        agl::Program, agl::UniformIndex<GLint>, std::any)>>;

template<typename Head, typename... Tail>
rtti_uniform_map make_rtti_uniforms(TypeList<Head, Tail...>) {
    auto t = make_rtti_uniforms(TypeList<Tail...>());
    t[std::type_index(typeid(Head))] = [](agl::Program p, agl::UniformIndex<GLint> ui, std::any a) {
        agl::uniform(p, ui, std::any_cast<Head>(a));
    };
    return t;
}

inline
rtti_uniform_map make_rtti_uniforms(TypeList<>) {
    return {};
}

const auto rtti_to_uniform = make_rtti_uniforms(TypeList<
        agl::Mat4,
        agl::Vec3,
        agl::Vec4,
        int>());

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
                auto type_idx = std::type_index(it->second.type());
                rtti_to_uniform.at(type_idx)(program, ui, it->second);
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
