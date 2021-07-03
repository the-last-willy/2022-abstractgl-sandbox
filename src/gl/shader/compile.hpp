#pragma once

#include "compile_status.hpp"
#include "info_log.hpp"
#include "shader.hpp"

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/exception/compilation_failure.hpp"

#include <stdexcept>
#include <string>

namespace tlw {
namespace gl {

GLboolean try_compile(const Shader& s) {
    glCompileShader(s);
    throw_if_error();
    return compile_status(s);
}

void compile(const Shader& s) {
    auto success = try_compile(s);
    if(!success) {
        throw CompilationFailure();
    }
}

}}
