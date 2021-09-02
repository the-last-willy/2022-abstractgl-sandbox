// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

#include "mesh.hpp"
#include "face_vertex_mesh/all.hpp"

#include <common/all.hpp>
#include <local/all.hpp>
#include "engine/data/all.hpp"
#include "engine/all.hpp"
#include "format/off/all.hpp"
#include "program/all.hpp"
#include "file.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard library.

#include <iostream>

//

using namespace chaine;

struct App : Program {
    eng::ShaderCompiler shader_compiler = {};

    Mesh mesh = {};
    std::shared_ptr<eng::Mesh> drawable_mesh = nullptr;

    std::shared_ptr<eng::Material> material = std::make_shared<eng::Material>();

    tlw::View view = {};
    eng::PerspectiveProjection projection = {};

    void init() override {
        shader_compiler.root = local::src_folder;

        load(material->program, shader_compiler, {
            {
                agl::vertex_shader_tag,
                "chaine/shader/solid.vs"
            },
            {
                agl::fragment_shader_tag,
                "chaine/shader/solid.fs"
            }
        });

        mesh.triangle_indices = { agl::Uvec3{0, 1, 2} };
        mesh.vertex_positions = {
            agl::vec3(0, 0, 1),
            agl::vec3(1, 0, 1),
            agl::vec3(0, 1, 1),
        };

        drawable_mesh = solid_mesh(mesh);

        drawable_mesh->primitives[0]->material = material;

        auto f = format::off::read(local::root_folder + "/data/queen.off");
        std::cout << std::endl;
    }

    void update(float dt) override {
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
    }

    void render() override {
        bind(*material);

        for(auto& primitive : drawable_mesh->primitives | common::views::indirect) {
            bind(primitive);
            bind(primitive, *primitive.material);
            uniform(primitive.material->program, "mvp", transform(projection) * inverse(transform(view)));
            eng::render(primitive);
        }
        unbind(*material);
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
