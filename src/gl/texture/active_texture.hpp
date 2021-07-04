#pragma once

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void active_texture(GLenum i) {
    glActiveTexture(GL_TEXTURE0 + i);
    throw_if_error();
}

}}
