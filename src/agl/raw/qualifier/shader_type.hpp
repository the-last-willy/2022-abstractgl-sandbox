#pragma once

#include <glad/glad.h>

namespace agl {

enum class ShaderType : GLenum {
    compute = GL_COMPUTE_SHADER,
    fragment = GL_FRAGMENT_SHADER,
    geometry = GL_GEOMETRY_SHADER,
    tess_control = GL_TESS_CONTROL_SHADER,
    tess_evaluation = GL_TESS_EVALUATION_SHADER,
    vertex = GL_VERTEX_SHADER,
};

}
