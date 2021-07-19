#pragma once

#include "gl/all.hpp"

#include "file.hpp"
#include "root.hpp"

#include <span>

namespace tlw {

struct Grass {
	Grass() {
		{
			auto positions = std::array{
				Vec3{-1.f, -1.f,  0.f}, Vec3{ 1.f,-1.f, 0.f}, Vec3{-1.f,  1.f,  0.f},
				Vec3{ 1.f,  1.f,  0.f}, Vec3{-1.f, 1.f, 0.f}, Vec3{ 1.f, -1.f,  0.f},
				Vec3{ 0.f, -1.f, -1.f}, Vec3{ 0.f, 1.f,-1.f}, Vec3{ 0.f, -1.f,  1.f},
				Vec3{ 0.f,  1.f,  1.f}, Vec3{ 0.f,-1.f, 1.f}, Vec3{ 0.f,  1.f, -1.f}};
			position_buffer = gl::Buffer(std::span(positions));
		}
		{
			program = gl::program({
				{
					GL_VERTEX_SHADER,
					file(root + "src/shader/terrain/grass.vs").c_str()
				},
                {
                    GL_FRAGMENT_SHADER,
                    file(root + "src/shader/terrain/grass.fs").c_str()
                }
            });
		}
		{
			{
				glVertexArrayVertexBuffer(
					vertex_array, 0, position_buffer, 0, stride(position_buffer));
				{
					auto l = gl::AttributeLocation(program, "position");
					glVertexArrayAttribFormat(
						vertex_array, l, 3, GL_FLOAT, GL_FALSE, 0);
					glVertexArrayBindingDivisor(vertex_array, l, 0);
					glVertexArrayAttribBinding(vertex_array, l, 0);
					glEnableVertexArrayAttrib(vertex_array, l);
				}
			}
		}
	}

	void instances(std::span<Mat4> s) {
		instance_count = GLuint(size(s));
		transform_buffer = gl::Buffer<Mat4>(s);
		{
			glVertexArrayVertexBuffer(
				vertex_array, 1, transform_buffer, 0, stride(transform_buffer));
			glVertexArrayBindingDivisor(vertex_array, 1, 1);
			auto l = gl::AttributeLocation(program, "model");
			for(int i = 0; i < 4; ++i) {
				glVertexArrayAttribFormat(
					vertex_array, l.index + i, 4, GL_FLOAT, GL_FALSE,
					i * stride(transform_buffer) / 4);
				glVertexArrayAttribBinding(vertex_array, l.index + i, 1);
				glEnableVertexArrayAttrib(vertex_array, l.index + i);
			}
		}
	}

	gl::Program program;
	gl::VertexArray vertex_array;

	gl::Buffer<Vec3> position_buffer;
	gl::Buffer<Mat4> transform_buffer;

	GLuint instance_count = 0;
};

}
