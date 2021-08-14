#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto ambient_lighting_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::ShaderType::vertex,
            file(tlw::root + "src/shader/gltf/deferred/ambient.vs")
        },
        {
            agl::ShaderType::fragment,
            file(tlw::root + "src/shader/gltf/deferred/ambient.fs")
        }});
    return m;
}

}
