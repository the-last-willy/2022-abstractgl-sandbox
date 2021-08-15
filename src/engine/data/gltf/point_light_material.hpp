#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto point_lighting_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::ShaderType::vertex,
            file(tlw::root + "src/shader/gltf/deferred/lighting/point.vs")
        },
        {
            agl::ShaderType::fragment,
            file(tlw::root + "src/shader/gltf/deferred/lighting/point.fs")
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::blend, []() {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE); });
    return m;
}

}
