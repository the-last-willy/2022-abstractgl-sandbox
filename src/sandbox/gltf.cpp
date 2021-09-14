// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

#include <agl/standard/all.hpp>
#include <local/all.hpp>
#include "engine/data/all.hpp"
#include "agl/format/gltf2/all.hpp"
#include "program/all.hpp"

// External libraries.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <range/v3/view/indirect.hpp>
#include <range/v3/view/map.hpp>

// Standard library.

#include <iostream>

//

struct DirectionalLight {
    agl::Vec3 direction = {};
    agl::Mat4 transform = {};

    std::shared_ptr<eng::Texture> shadow_map = std::make_shared<eng::Texture>();;
};

struct SpotLight {
    agl::Vec3 direction = {};
    agl::Mat4 transform = {};

    std::shared_ptr<eng::Texture> shadow_map = std::make_shared<eng::Texture>();;
};

struct PointLight {
    agl::Vec3 position = {};
};

struct GltfProgram;

inline
void load_model(GltfProgram& program, const std::string& filepath);

struct GltfProgram : Program {
    std::vector<std::string> files = {
        "D:/data/sample/gltf2/box/Box/glTF/Box.gltf",
    };

    eng::ShaderCompiler shader_compiler = {};

    // glTF file.
    format::gltf2::Content scene = {};

    // Default sampler.
    agl::Sampler default_sampler;

    // Ambient lighting.
    eng::Material ambient_light_mat = {};

    // Active camera.
    std::shared_ptr<eng::Camera> active_camera = {};

    // Player camera.
    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};

    agl::engine::RenderPass ambient_light_pass;
    
    void init() override {
        { // Shader compiler.
            shader_compiler.root = local::shader_folder;
        }

        { // Ambient light pass.
            auto& program = *(ambient_light_pass.program = std::make_shared<eng::Program>());
            load(program, shader_compiler, {
                {
                    agl::vertex_shader_tag,
                    "/forward/ambient.vs"
                }, {
                    agl::fragment_shader_tag,
                    "/forward/ambient.fs"
                }
            });
            program.capabilities.emplace_back(agl::Capability::cull_face, []() {
                glCullFace(GL_BACK); });
            program.capabilities.emplace_back(agl::Capability::depth_test, []() {
                glDepthFunc(GL_LESS); });
        }
        load_model(*this, files[0]);

        {
            for(auto& m : scene.meshes | ranges::views::values) {
                subscribe(ambient_light_pass, m);
            }
        }

        { // Camera.
            // SCENE CAMERA DISABLED>
            if constexpr(true /* empty(scene.cameras) */) {
                auto& c = *(active_camera = std::make_shared<eng::Camera>());
                if(auto pp = std::get_if<eng::PerspectiveProjection>(&c.projection)) {
                    pp->aspect_ratio = 16.f / 9.f;
                }
            } else {
                active_camera = scene.cameras.begin()->second;
            }
        }
    }

    void update(float) override {
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
                auto direction = (rotation(view) * agl::rotation_y(agl::constant::pi / 2.f))[2].xyz();
                view.position = view.position - direction / 10.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_D)) {
                auto direction = (rotation(view) * agl::rotation_y(agl::constant::pi / 2.f))[2].xyz();
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

        auto inv_v = transform(view);
        auto v = inverse(inv_v);

        auto vp = transform(*active_camera) * v;

        { // Ambient light pass.
            ambient_light_pass.uniforms["mvp_transform"] = std::make_shared<eng::Uniform<agl::Mat4>>(vp);
            agl::engine::render(ambient_light_pass);
        }
    }
};

inline
void load_model(GltfProgram& program, const std::string& filepath) {
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(
        &model, &err, &warn, 
        filepath
        );

    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to open GLTF file." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    program.scene = format::gltf2::fill(model);

    { // Normalizing cameras.
        for(auto& c : program.scene.cameras | ranges::views::values | ranges::views::indirect) {
            if(auto pp = std::get_if<eng::PerspectiveProjection>(&c.projection)) {
                pp->aspect_ratio = 16.f / 9.f;
            }
        }
    }

    { // Normalizing materials.
        for(auto& mesh : program.scene.meshes | ranges::views::values | ranges::views::indirect)
        for(auto& primitive : mesh.primitives | ranges::views::indirect) {
            if(!primitive.material) {
                primitive.material = std::make_shared<eng::Material>();
            }

            auto& m = *primitive.material;

            m.program.program = program.ambient_light_pass.program->program;
        }
    }

    { // Vertex attribute plumbing.
        for(auto& mesh : program.scene.meshes | ranges::views::values | ranges::views::indirect)
        for(auto& primitive : mesh.primitives | ranges::views::indirect) {
            bind(primitive, *primitive.material);
        }
    }
}

void throwing_main() {
    auto p = GltfProgram();
    run(p);
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
