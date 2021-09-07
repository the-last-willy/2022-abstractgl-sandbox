#pragma once

#include "agl/engine/all.hpp"

#include <agl/all.hpp>

namespace eng {

inline
Texture default_albedo_texture(agl::Vec3 value) {
    auto texture = Texture();
    texture.texture = create(agl::TextureTarget::_2d);
    storage(
        texture.texture,
        GL_RGB8, agl::Width(1), agl::Height(1));
    image(
        texture.texture,
        agl::Level(0),
        0, 0, agl::Width(1), agl::Height(1),
        GL_RGB, GL_FLOAT,
        as_bytes(std::span(data(value), size(value))));
    return texture;
}

}
