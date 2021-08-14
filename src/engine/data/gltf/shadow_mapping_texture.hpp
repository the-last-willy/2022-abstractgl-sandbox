#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto shadow_mapping_texture(int resolution) {
    auto t = create(agl::TextureTarget::_2d);
    mag_filter(t, GL_NEAREST);
    min_filter(t, GL_NEAREST);
    parameter(t, agl::TextureParameter::wrap_s, GL_REPEAT); 
    parameter(t, agl::TextureParameter::wrap_t, GL_REPEAT);  
    storage(
        t,
        GL_DEPTH_COMPONENT16,
        agl::Width(resolution),
        agl::Height(resolution));
    return t;
}

}
