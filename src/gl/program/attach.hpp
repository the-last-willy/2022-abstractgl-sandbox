#pragma once

#include "program.hpp"

#include "gl/exception/error/throw_if_error.hpp"

namespace tlw {
namespace gl {

inline
void attach(const Program& p, GLuint shader) {
    glAttachShader(p, shader);
    throw_if_error();
}

}}

