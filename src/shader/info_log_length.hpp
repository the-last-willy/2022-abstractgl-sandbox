#pragma once

#include "shader.hpp"

namespace tlw {

GLint info_log_length(const Shader& s) {
    GLint value;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &value);
    return value;
}

}
