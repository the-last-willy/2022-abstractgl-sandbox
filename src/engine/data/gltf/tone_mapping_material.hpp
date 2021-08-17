#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto tone_mapping_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            file(tlw::root + "src/shader/gltf/tone_mapping.vs")
        },
        {
            agl::fragment_shader_tag,
            file(tlw::root + "src/shader/gltf/tone_mapping.fs")
        }});
    return m;
}

}
