#pragma once

#include "agl/raw/qualifier/shader.hpp"
#include "agl/raw/qualifier/shader_type.hpp"

namespace agl {

inline
Shader create_shader(ShaderType t) {
    return Shader(glCreateShader(static_cast<GLenum>(t)));
}

inline
Shader shader(ShaderType t) {
    return create_shader(t);
}

}
