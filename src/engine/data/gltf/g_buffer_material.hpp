#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto g_buffer_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::ShaderType::vertex,
            file(tlw::root + "src/shader/gltf/deferred/g_buffer.vs"
            )
        },
        {
            agl::ShaderType::fragment,
            file(tlw::root + "src/shader/gltf/deferred/g_buffer.fs"
            )
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::depth_test, []() {
            glDepthFunc(GL_LESS); });
    return m;
}

}
