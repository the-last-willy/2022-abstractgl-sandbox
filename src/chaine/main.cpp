// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

#include "mesh.hpp"

#include <local/all.hpp>
#include "engine/data/all.hpp"
#include "engine/all.hpp"
#include "format/gltf2/all.hpp"
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
            agl::vec3(0, 0, 0),
            agl::vec3(1, 0, 0),
            agl::vec3(0, 1, 0),
        };

        drawable_mesh = solid_mesh(mesh);
    }

    void update(float dt) override {
        
    }

    void render() override {
        bind(*material);
        for(auto& primitive : drawable_mesh->primitives | common::views::indirect) {
            bind(primitive.vertex_array);
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
