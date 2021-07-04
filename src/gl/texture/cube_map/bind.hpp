#pragma once

#include "cube_map.hpp"

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void bind(const CubeMap& cm) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, cm);
    throw_if_error();
}

}}
