#pragma once

#include "compile_status.hpp"
#include "shader.hpp"

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/exception/compilation_failure.hpp"

namespace tlw {
namespace gl {

template<typename Any>
GLboolean try_compile(const Shader<Any>& s) {
    glCompileShader(s);
    throw_if_error();
    return compile_status(s);
}

template<typename Any>
void compile(const Shader<Any> s) {
    auto success = try_compile(s);
    if(!success) {
        throw CompilationFailure();
    }
}

}}
