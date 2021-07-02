#pragma once

#include "gl/exception/error/throw_if_error.hpp"

#include <glad/glad.h>

#include <iostream>
#include <stdexcept>

namespace tlw {
namespace gl {

void draw_arrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
    throw_if_error();
}

void draw_elements(GLenum mode, GLsizei count, GLenum type, const void* offset) {
    glDrawElements(mode, count, type, offset);
    throw_if_error();
}

}}
