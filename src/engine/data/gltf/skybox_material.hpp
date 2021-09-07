#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto skybox_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/skybox.vs")
        },
        {
            agl::fragment_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/skybox.fs")
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::texture_cube_map_seamless);
    return m;
}

}
