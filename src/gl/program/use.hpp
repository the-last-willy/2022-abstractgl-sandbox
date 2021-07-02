#pragma once

#include "program.hpp"

#include "gl/exception/error/throw_if_error.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

inline
void use(const Program& p) {
    glUseProgram(p);
    throw_if_error();
}

}}
