#pragma once

#include "shader.hpp"

namespace tlw {
namespace gl {

template<typename Any>
GLint info_log_length(const Shader<Any>& s) {
    GLint value;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &value);
    return value;
}

}}
