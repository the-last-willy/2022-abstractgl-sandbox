#pragma once

#include "texture.hpp"

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void bind_to_cube_map(const Texture& t) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, t);
    throw_if_error();
}

inline
void bind_to_texture2(const Texture& t) {
    glBindTexture(GL_TEXTURE_2D, t);
    throw_if_error();
}

}}
