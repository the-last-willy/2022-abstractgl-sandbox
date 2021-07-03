#pragma once

#include "texture.hpp"

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void bind_to_texture2(const Texture& t) {
    glBindTexture(GL_TEXTURE_2D, t);
    throw_if_error();
}

}}
