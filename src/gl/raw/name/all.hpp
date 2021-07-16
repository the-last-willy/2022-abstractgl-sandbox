#pragma once

#include <glad/glad.hpp>

namespace tlw::gl {

struct Name {
	operator GLuint() const noexcept {
		return name;
	}

	operator GLuint&() noexcept {
		return name;
	}

	GLuint name = 0;
};

struct VertexArrayName : Name {};

}
