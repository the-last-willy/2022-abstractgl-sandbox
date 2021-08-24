#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto pbr_lighting_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            file(tlw::root + "src/shader/gltf/deferred/brdf/point_light.vs")
        },
        {
            agl::fragment_shader_tag,
            file(tlw::root + "src/shader/gltf/deferred/brdf/point_light.fs")
        }});
    
    m.program.capabilities.emplace_back(
        agl::Capability::blend, []() {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE); });
    return m;
}

}
