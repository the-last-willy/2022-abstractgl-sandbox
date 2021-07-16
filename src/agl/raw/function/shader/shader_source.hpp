#pragma once

#include "agl/raw/qualifier/shader.hpp"

#include <string_view> 

namespace agl {

inline
void shader_source(Shader s, GLsizei count, const GLchar* const* string, const GLint* length) {
    glShaderSource(
        static_cast<GLuint>(s),
        count,
        string,
        length);
}

inline
void source(Shader sh, std::string_view source) {
    // Credits to Sp0nji.
    auto d = data(source);
    auto si = static_cast<GLint>(size(source));
    shader_source(sh, 1, &d, &si);
}

}
