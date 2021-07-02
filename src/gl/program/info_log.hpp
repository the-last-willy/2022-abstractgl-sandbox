#pragma once

#include "info_log_length.hpp"
#include "program.hpp"

#include <string>

namespace tlw {
namespace gl {

inline
std::string info_log(const Program& p) {
    auto l = info_log_length(p);
    auto str = std::string(l, '\0');
    glGetShaderInfoLog(p, l, NULL, str.data());
    return str;
}

}}
