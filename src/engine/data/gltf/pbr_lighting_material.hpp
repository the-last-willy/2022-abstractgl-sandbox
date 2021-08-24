#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto pbr_lighting_material(const eng::ShaderCompiler& sc) {
    auto m = eng::Material();
    load(m.program, sc, {
        {
            agl::vertex_shader_tag,
            "gltf/deferred/brdf/point_light.vs"
        },
        {
            agl::fragment_shader_tag,
            "gltf/deferred/brdf/point_light.fs"
        }});
    if(!agl::link_status(m.program.program)) {
        throw std::runtime_error("Failed to load material.");
    }
    m.program.capabilities.emplace_back(
        agl::Capability::blend, []() {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE); });
    return m;
}

}
