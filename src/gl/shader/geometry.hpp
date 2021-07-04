#pragma once

#include "shader.hpp"

namespace tlw {
namespace gl {

struct GeometryShaderSubtraits {
    static GLuint create() {
        return glCreateShader(GL_GEOMETRY_SHADER);
    }
};

using GeometryShader = Shader<GeometryShaderSubtraits>;

}}
