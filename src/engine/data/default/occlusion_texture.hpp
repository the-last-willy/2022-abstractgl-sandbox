#pragma once

<<<<<<< HEAD
#include "agl/engine/all.hpp"
=======
#include "agl/engine/texture.hpp"
>>>>>>> 89a92eccf06d4d9f62d983c3ccf54dd4f75c66fd

#include <agl/all.hpp>

namespace eng {

inline
Texture default_occlusion_texture(float value) {
    auto texture = Texture();
    texture.texture = create(agl::TextureTarget::_2d);
    storage(
        texture.texture,
        GL_R16F, agl::Width(1), agl::Height(1));
    image(
        texture.texture,
        agl::Level(0),
        0, 0, agl::Width(1), agl::Height(1),
        GL_RED, GL_FLOAT,
        as_bytes(std::span(&value, 1)));
    return texture;
}

}
