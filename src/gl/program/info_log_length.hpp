#pragma once

#include "program.hpp"

namespace tlw {
namespace gl {

inline
GLint info_log_length(const Program& p) {
    GLint value;
    glGetShaderiv(p, GL_INFO_LOG_LENGTH, &value);
    return value;
}

}}
