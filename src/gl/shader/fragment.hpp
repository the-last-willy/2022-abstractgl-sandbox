#pragma once

#include "shader.hpp"

namespace tlw {
namespace gl {

struct FragmentShaderSubtraits {
    static GLuint create() {
        return glCreateShader(GL_FRAGMENT_SHADER);
    }
};

using FragmentShader = Shader<FragmentShaderSubtraits>;

}}
