#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto cube_shadow_mapping_texture(int resolution) {
    auto t = create(agl::TextureTarget::cube_map_array);
    mag_filter(t, GL_NEAREST);
    min_filter(t, GL_NEAREST);
    parameter(t, agl::TextureParameter::wrap_s, GL_CLAMP_TO_EDGE);
    parameter(t, agl::TextureParameter::wrap_t, GL_CLAMP_TO_EDGE);
    storage(
        t, 1, GL_DEPTH_COMPONENT16, 
        agl::Width(resolution),
        agl::Height(resolution),
        agl::Depth(6));
    return t;
}

}
