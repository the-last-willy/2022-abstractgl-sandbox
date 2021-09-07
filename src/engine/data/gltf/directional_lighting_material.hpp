#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto directional_lighting_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/deferred/directional_lighting.vs")
        },
        {
            agl::fragment_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/deferred/directional_lighting.fs")
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::blend, []() {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE); });
    return m;
}

}
