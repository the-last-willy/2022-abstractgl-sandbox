#pragma once

#include "../solid_uv_sphere.hpp"
#include "common/glsl/solid_renderer/solid_renderer.hpp"

 namespace gizmo {

 inline
 gl::VertexArrayObj
 vertex_array(
	const Solid_UV_Sphere& suvs,
	const glsl::SolidRenderer& sr)
{
	auto va = gl::VertexArrayObj();
	auto binding_index_count = GLuint(0);
	// Normals, positions.
	if(gl::GetNamedBufferParameter(suvs.normals_positions, gl::BUFFER_SIZE) > 0) {
		auto bindingindex = binding_index_count++;
		gl::VertexArrayVertexBuffer(va,
			bindingindex,
			suvs.normals_positions,
			0, sizeof(glm::vec3));
		{ // Normals.
			gl::VertexArrayAttribFormat(va,
				sr.normal,
				3, GL_FLOAT,
				GL_FALSE, 0);
			gl::VertexArrayAttribBinding(va,
				sr.normal,
				bindingindex);
			gl::EnableVertexArrayAttrib(va,
				sr.normal);
		}
		{ // Positions.
			gl::VertexArrayAttribFormat(va,
				sr.position,
				3, GL_FLOAT,
				GL_FALSE, 0);
			gl::VertexArrayAttribBinding(va,
				sr.position,
				bindingindex);
			gl::EnableVertexArrayAttrib(va,
				sr.position);
		}

	}
	{ // Elements.
		gl::VertexArrayElementBuffer(va,
			suvs.elements);
	}
	return va;
 }

 }