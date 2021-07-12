#pragma once

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/program/program.hpp"

#include <optional>
#include <stdexcept>

namespace tlw {
namespace gl {

inline
std::optional<GLint> optional_uniform_location(const Program& p, const GLchar* name) {
    auto l = glGetUniformLocation(p, name);
    throw_if_error();
    if(l == -1) {
        return std::nullopt;
    } else {
        return l;
    }
}

inline
GLint uniform_location(const Program& p, const GLchar* name) {
    auto l = optional_uniform_location(p, name);
    {
        if(*l == -1) {
            throw std::runtime_error(
                std::string()
                + "Program: \""
                + name 
                + "\" is not an active uniform variable.");
        }
    }
    return *l;
}

}}
