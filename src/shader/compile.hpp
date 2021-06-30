#pragma once

#include "compile_status.hpp"
#include "info_log.hpp"
#include "shader.hpp"

#include <stdexcept>
#include <string>

namespace tlw {

std::string compile(const Shader& s) {
    glCompileShader(s);
    if(compile_status(s)) {
        return info_log(s);
    } else {
        throw std::runtime_error(
            "Shader: Compilation failure:\n" + info_log(s));
    }
}

}
