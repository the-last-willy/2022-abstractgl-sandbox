#pragma once

#include "shader.hpp"

namespace tlw {

GLboolean compile_status(const Shader& s) {
    GLint value;
    glGetShaderiv(s, GL_COMPILE_STATUS, &value);
    return static_cast<GLboolean>(value);
}

}
