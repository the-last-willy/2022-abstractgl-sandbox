#pragma once

#include "texture2.hpp"

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void bind(const Texture2& t) {
    glBindTexture(GL_TEXTURE_2D, t);
    throw_if_error();
}

}}
