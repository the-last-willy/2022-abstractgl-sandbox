#pragma once

#include "shader.hpp"

#include <string>

namespace tlw {
namespace gl {

template<typename Any>
void source(const Shader<Any>& sh, const std::string& str) {
    auto string = str.data();
    auto length = static_cast<GLint>(std::size(str));
    glShaderSource(sh, 1, &string, &length);
}

}}
