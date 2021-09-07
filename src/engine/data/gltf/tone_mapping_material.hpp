#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto tone_mapping_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/tone_mapping.vs")
        },
        {
            agl::fragment_shader_tag,
            agl::standard::string(local::src_folder + "shader/gltf/tone_mapping.fs")
        }});
    return m;
}

}
