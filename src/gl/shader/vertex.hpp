#pragma once

#include "shader.hpp"

namespace tlw {
namespace gl {

struct VertexShaderSubtraits {
    static GLuint create() {
        return glCreateShader(GL_VERTEX_SHADER);
    }
};

using VertexShader = Shader<VertexShaderSubtraits>;

}}
