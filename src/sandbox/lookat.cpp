
#include "gl/all.hpp"
#include "scene/all.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <span>
#include <vector>

using namespace tlw;

Vec2 previous_cursor_pos;
Vec2 current_cursor_pos;

static void cursor_position_callback(GLFWwindow*, double x, double y) {
	current_cursor_pos = {float(x), float(y)};
}

static void error_callback(int error, const char* description)
{
	(void) error;
	std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void) scancode;
	(void) mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int throwing_main() {
	std::srand(static_cast<unsigned>(std::time(0)));

	GLFWwindow* window;
	{
		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
			exit(EXIT_FAILURE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(1280, 720, "Procedural Terrain Generation", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetKeyCallback(window, key_callback);

		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			previous_cursor_pos = {float(x), float(y)};
			current_cursor_pos = {float(x), float(y)};
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
			printf("Failed to initialize OpenGL context");
			return -1;
		}

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(&gl::error_callback, NULL);
		gl::throw_if_error();
	}

	auto terrain_size = 1 << (12 - 1);

//	auto quad_positions = gl::Buffer<Vec3>();
//	{
//		auto positions = std::array{
//			Vec3{-1.f, -1.f,  0.f}, Vec3{ 1.f,-1.f, 0.f}, Vec3{-1.f,  1.f,  0.f},
//			Vec3{ 1.f,  1.f,  0.f}, Vec3{-1.f, 1.f, 0.f}, Vec3{ 1.f, -1.f,  0.f},
//			Vec3{ 0.f, -1.f, -1.f}, Vec3{ 0.f, 1.f,-1.f}, Vec3{ 0.f, -1.f,  1.f},
//			Vec3{ 0.f,  1.f,  1.f}, Vec3{ 0.f,-1.f, 1.f}, Vec3{ 0.f,  1.f, -1.f}};
//		quad_positions = gl::Buffer(std::span(positions));
//	}
//	glObjectLabel(GL_BUFFER, quad_positions, 6, "grass");

//	auto grass_transforms = gl::Buffer<Mat4>(1'000, GL_MAP_WRITE_BIT);
//	{
//		auto mapping = gl::BufferMapping(grass_transforms, GL_WRITE_ONLY);
//		for(int i = 0; i < 1'000; ++i) {
//			auto x = float(std::rand() % terrain_size) / terrain_size;
//			auto y = 0.f;
//			auto z = float(std::rand() % terrain_size) / terrain_size;
//			mapping[i] = translation(x, y, z) * scaling(0.01f);
//		}
//	}

//	auto grass_program = gl::program({
//         {
//             GL_VERTEX_SHADER,
//             file(root + "src/shader/terrain/grass.vs").c_str()
//         },
//         {
//             GL_FRAGMENT_SHADER,
//             file(root + "src/shader/terrain/grass.fs").c_str()
//         }
//     });

//	auto grass_vertex_array = gl::VertexArray();
//	{
//		{
//			glVertexArrayVertexBuffer(
//				grass_vertex_array, 1, grass_transforms,
//				0, stride(grass_transforms));
//			glVertexArrayBindingDivisor(grass_vertex_array, 1, 1);
//			auto l = gl::AttributeLocation(grass_program, "model");
//			for(int i = 0; i < 4; ++i) {
//				glVertexArrayAttribFormat(
//					grass_vertex_array, l.index + i, 4, GL_FLOAT, GL_FALSE,
//					i * stride(grass_transforms) / 4);
//				glVertexArrayAttribBinding(grass_vertex_array, l.index + i, 1);
//				glEnableVertexArrayAttrib(grass_vertex_array, l.index + i);
//			}
//		}
//		{
//			glVertexArrayVertexBuffer(
//				grass_vertex_array, 0, quad_positions, 0, stride(quad_positions));
//			{
//				auto l = gl::AttributeLocation(grass_program, "position");
//				glVertexArrayAttribFormat(
//					grass_vertex_array, l, 3, GL_FLOAT, GL_FALSE, 0);
//				glVertexArrayBindingDivisor(grass_vertex_array, l, 0);
//				glVertexArrayAttribBinding(grass_vertex_array, l, 0);
//				glEnableVertexArrayAttrib(grass_vertex_array, l);
//			}
//		}
//	}

	auto projection = PerspectiveProjection();
	auto view = View();
	view.rotation_center = Vec3{0.f, 0.f, -1.f};
	view.pitch += 0.2f;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	auto grass = Grass();

	while(!glfwWindowShouldClose(window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1280, 720);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			Vec2 d = current_cursor_pos - previous_cursor_pos;
			view.yaw += d.at(0) / 500.f;
			view.pitch += d.at(1) / 500.f;

			previous_cursor_pos = current_cursor_pos;
		}
		{
			if(glfwGetKey(window, GLFW_KEY_S)) {
				view.position = view.position
				                + xyz(transform(view) * Vec4{0.f, 0.f, -1.f, 0.f});
			}
			if(glfwGetKey(window, GLFW_KEY_W)) {
				view.position = view.position
				                + xyz(transform(view) * Vec4{0.f, 0.f, 1.f, 0.f});
			}
		}

		{
			glDepthFunc(GL_LESS);
			glEnable(GL_DEPTH_TEST);

			auto v = inverse(transform(view));
			auto p = transform(projection);

			auto vp = p * v;
			auto mv = v;
			auto mvp = p * mv;

			{
				gl::use(grass.program);
				gl::bind(grass.vertex_array);

				gl::Uniform(grass.program, "vp") = vp;

				glDrawArraysInstanced(GL_TRIANGLES, 0, 12, 1'000);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

int main() {
	try {
		return throwing_main();
	} catch(const gl::Error& e) {
		std::cerr << "tlw::gl::Error(" << std::hex << e.category() << ")" << std::endl;
	} catch(const gl::CompilationFailure&) {
		std::cerr << "tlw::gl::CompilationFailure" << std::endl;
	} catch(const gl::IncompleteFrameBuffer& i) {
		std::cerr << "tlw::gl::IncompleteFrameBuffer: " << std::hex << i.category() << std::endl;
	} catch(const gl::InvariantViolation&) {
		std::cerr << "tlw::gl::InvariantViolation" << std::endl;
	} catch(const gl::LinkageFailure&) {
		std::cerr << "tlw::gl::LinkageFailure" << std::endl;
	} catch(const std::exception& e) {
		std::cerr << "std::exception: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unhandled exception." << std::endl;
	}
	return -1;
}
