#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto tone_mapping_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::ShaderType::vertex,
            file(tlw::root + "src/shader/gltf/tone_mapping.vs")
        },
        {
            agl::ShaderType::fragment,
            file(tlw::root + "src/shader/gltf/tone_mapping.fs")
        }});
    return m;
}

}
