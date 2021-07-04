#pragma once

#include "info_log_length.hpp"
#include "shader.hpp"

#include <string>

namespace tlw {
namespace gl {

template<typename Any>
std::string info_log(const Shader<Any>& s) {
    auto l = info_log_length(s);
    auto str = std::string(l, '\0');
    glGetShaderInfoLog(s, l, NULL, str.data());
    return str;
}

}}
