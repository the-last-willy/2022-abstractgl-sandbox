#pragma once

#include "program.hpp"

#include "gl/exception/error/throw_if_error.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

inline
GLint uniform_location(const Program& p, const GLchar* name) {
    auto l = glGetUniformLocation(p, name);
    throw_if_error();
    {
        if(l == -1) {
            throw std::runtime_error(
                std::string() + "Program: \"" + name + "\" is not an active uniform variable.");
        }
    }
    return l;
}

}}
