#pragma once

#include "program.hpp"

namespace tlw {
namespace gl {

inline
GLint uniform_location(const Program& p, const GLchar* name) {
    auto l = glGetUniformLocation(p, name);
    {
        if(l == -1) {
            throw std::runtime_error(
                std::string() + "Program: \"" + name + "\" is not an active uniform variable.");
        }
    }
    {
        if(glGetError() == GL_INVALID_OPERATION) {
            throw std::runtime_error(
                "Program must be linked.");
        }
    }
    return l;
}

}}
