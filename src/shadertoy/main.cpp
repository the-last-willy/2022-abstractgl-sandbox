// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

#include "all.hpp"

#include <common/all.hpp>
#include <local/all.hpp>
#include <engine/all.hpp>
#include <program/all.hpp>
#include "file.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard library.

#include <iostream>

//

namespace shadertoy {}

using namespace shadertoy;

struct App : Program {
    eng::ShaderCompiler shader_compiler = {};

    tlw::View view = {};

    agl::VertexArray empty_vertex_array = agl::create(agl::vertex_array_tag);
    eng::Program program = {};

    float time = 0.f;

    bool shader_loaded = false;

    void load_shader() {
        try {
            { // Render pass.
                shader_compiler.root = local::src_folder;
                load(program, shader_compiler, {
                    {
                        agl::vertex_shader_tag,
                        "shadertoy/shader/shadertoy.vs"
                    },
                    {
                        agl::fragment_shader_tag,
                        "shadertoy/shader/blob.glsl"
                    }
                });
            }
            shader_loaded = true;
        } catch(...) {
            shader_loaded = false;
        }
        if(shader_loaded) {
            uniform(program, "iResolution", agl::vec3(
                static_cast<float>(window.width()),
                static_cast<float>(window.height()),
                0.f));
        }
    }

    void init() override {
        load_shader();
    }

    void update(float dt) override {
        time += dt;

        if(glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_1)) {
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            agl::Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw += d[0] / 500.f;
            view.pitch += d[1] / 500.f;

            previous_cursor_pos = current_cursor_pos;
        } else {
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window.window, GLFW_KEY_A)) {
                auto direction = (rotation(view) * agl::rotation_y(agl::pi / 2.f))[2].xyz();
                view.position = view.position - direction / 10.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_D)) {
                auto direction = (rotation(view) * agl::rotation_y(agl::pi / 2.f))[2].xyz();
                view.position = view.position + direction / 10.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_S)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position - direction / 10.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_W)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position + direction / 10.f;
            }
        }
        { // Shader hot reloading.
            if(glfwGetKey(window.window, GLFW_KEY_R)) {
                load_shader();
            }
        }

        if(shader_loaded) {
            double xpos, ypos;
            glfwGetCursorPos(window.window, &xpos, &ypos);
            auto pressed = glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_1);
            uniform(program, "iMouse", agl::vec4(
                static_cast<float>(xpos),
                static_cast<float>(ypos),
                static_cast<float>(pressed),
                0.f
            ));

            uniform(program, "view", inverse(transform(view)));
        }
    }

    void render() override {
        clear(agl::default_framebuffer, agl::color_tag, {0., 0., 0., 1.});
        if(shader_loaded) {
            bind(empty_vertex_array);
            bind(program);
            uniform(program, "iTime", time);
            draw_arrays(
                agl::DrawMode::triangles,
                agl::Offset<GLint>(0),
                agl::Count<GLsizei>(6));
        }   
    }
};

void throwing_main() {
    auto a = App();
    run(a);
}

int main() {
    try {
        throwing_main();
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}
