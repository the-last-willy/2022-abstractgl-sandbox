#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto point_lighting_material(const eng::ShaderCompiler& sc) {
    auto m = eng::Material();
    load(m.program, sc, {
        {
            agl::vertex_shader_tag,
            "gltf/deferred/lighting/point.vs"
        },
        {
            agl::fragment_shader_tag,
            "gltf/deferred/lighting/point.fs"
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::blend, []() {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE); });
    return m;
}

}
