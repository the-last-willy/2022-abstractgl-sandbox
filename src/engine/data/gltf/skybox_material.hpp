#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto skybox_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::ShaderType::vertex,
            file(tlw::root + "src/shader/gltf/skybox.vs")
        },
        {
            agl::ShaderType::fragment,
            file(tlw::root + "src/shader/gltf/skybox.fs")
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::texture_cube_map_seamless);
    return m;
}

}
