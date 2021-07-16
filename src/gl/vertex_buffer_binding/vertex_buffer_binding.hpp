#pragma once

#include "gl/buffer/buffer.hpp"
#include "gl/vertex_array/vertex_array.hpp"

namespace tlw::gl {

class VertexBufferBinding {
	const GLuint vertex_array;
	const GLuint index;

public:
	VertexBufferBinding(const VertexArray& va, GLuint index) noexcept
		: vertex_array(va)
		, index(index)
	{}

};

}