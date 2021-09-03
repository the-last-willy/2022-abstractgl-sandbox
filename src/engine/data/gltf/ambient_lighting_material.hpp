#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto ambient_lighting_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            file(tlw::root + "src/shader/gltf/deferred/ambient.vs")
        },
        {
            agl::fragment_shader_tag,
            file(tlw::root + "src/shader/gltf/deferred/ambient.fs")
        }});
    return m;
}

}
