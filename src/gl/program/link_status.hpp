#pragma once

#include "program.hpp"

namespace tlw {
namespace gl {

inline
GLboolean link_status(const Program& p) {
    GLint i;
    glGetProgramiv(p, GL_LINK_STATUS, &i);
    return static_cast<GLboolean>(i);
}

}}
