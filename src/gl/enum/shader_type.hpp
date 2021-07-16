#pragma once

#include <glad/glad.h>

namespace tlw::gl {

enum class ShaderType : GLenum {
	fragment = GL_FRAGMENT_SHADER,
	geometry = GL_GEOMETRY_SHADER,
	tessellation_control = GL_TESS_CONTROL_SHADER,
	tessellation_evaluation = GL_TESS_EVALUATION_SHADER,
	vertex = GL_VERTEX_SHADER
};

}
