#pragma once

#include "shader.hpp"

namespace tlw {
namespace gl {

template<typename Any>
GLboolean compile_status(const Shader<Any>& s) {
    GLint value;
    glGetShaderiv(s, GL_COMPILE_STATUS, &value);
    return static_cast<GLboolean>(value);
}

}}
