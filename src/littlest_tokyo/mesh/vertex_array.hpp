#pragma once

#include "mesh.hpp"

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"
#include "common/glsl/solid_renderer/solid_renderer.hpp"

inline
gl::VertexArrayObj
vertex_array(
    const Mesh& m,
    const glsl::SolidRenderer& sr)
{
    auto va = gl::VertexArrayObj();
    auto binding_index_count = GLuint(0);
    // Indices.
    if(gl::GetNamedBufferParameter(m.indices, gl::BUFFER_SIZE) > 0) {
        gl::VertexArrayElementBuffer(va, m.indices);
    }
    // Normals.
    if(gl::GetNamedBufferParameter(m.normals, gl::BUFFER_SIZE) > 0) {
        auto bindingindex = binding_index_count++;
        gl::VertexArrayAttribFormat(va,
            sr.normal,
            3, GL_FLOAT,
            GL_FALSE, 0);
        gl::VertexArrayVertexBuffer(va,
            bindingindex,
            m.normals,
            0, sizeof(glm::vec3));
        gl::VertexArrayAttribBinding(va,
            sr.normal,
            bindingindex);
        gl::EnableVertexArrayAttrib(va,
            sr.normal);
    }
    // Positions.
    if(gl::GetNamedBufferParameter(m.positions, gl::BUFFER_SIZE) > 0) { 
        auto bindingindex = binding_index_count++;
        gl::VertexArrayAttribFormat(va,
            sr.position,
            3, GL_FLOAT,
            GL_FALSE, 0);
        gl::VertexArrayVertexBuffer(va,
            bindingindex,
            m.positions,
            0, sizeof(glm::vec3));
        gl::VertexArrayAttribBinding(va,
            sr.position,
            bindingindex);
        gl::EnableVertexArrayAttrib(va,
            sr.position);
    }
	// Texcoords0.
	if(gl::GetNamedBufferParameter(m.texcoords0, gl::BUFFER_SIZE) > 0) {
		auto bindingindex = binding_index_count++;
		gl::VertexArrayVertexBuffer(va,
			bindingindex,
			m.texcoords0,
			0, sizeof(glm::vec3));
		gl::VertexArrayAttribFormat(va,
			sr.texcoords0,
			3, GL_FLOAT,
			GL_FALSE, 0);
		gl::VertexArrayAttribBinding(va,
			sr.texcoords0,
			bindingindex);
		gl::EnableVertexArrayAttrib(va,
			sr.texcoords0);
	}
    return va;
}
