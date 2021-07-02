#pragma once

#include "error.hpp"

namespace tlw {
namespace gl {

void throw_if_error() {
    auto e = glGetError();
    if(e != GL_NO_ERROR) {
        throw Error(e);
    }
}

}}
