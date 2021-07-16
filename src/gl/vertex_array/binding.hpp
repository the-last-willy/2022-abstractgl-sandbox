#pragma once

#include "vertex_array.hpp"

#include "gl/vertex_buffer_binding/vertex_buffer_binding.hpp"

namespace tlw::gl {

inline
auto binding(const VertexArray& a, GLuint index) {
	return VertexBufferBinding(a, index);
}

}
