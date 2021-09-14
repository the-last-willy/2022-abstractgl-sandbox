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
        "C:/Users/Willy/Desktop/data/sample/gltf2/Box/glTF/Box.gltf",
    };

    eng::ShaderCompiler shader_compiler = {};

    std::shared_ptr<eng::Primitive> fullscreen_prim = std::make_shared<eng::Primitive>();

    // glTF file.
    format::gltf2::Content scene = {};

    // Default sampler.
    agl::Sampler default_sampler;

    // Default material.
    std::shared_ptr<eng::Material> default_material = std::make_shared<eng::Material>();

    // Default textures.
    std::shared_ptr<eng::Texture> default_albedo_tex = std::make_shared<eng::Texture>();
    std::shared_ptr<eng::Texture> default_emissive_tex = std::make_shared<eng::Texture>();
    std::shared_ptr<eng::Texture> default_normal_map_tex = std::make_shared<eng::Texture>();
    std::shared_ptr<eng::Texture> default_occlusion_tex = std::make_shared<eng::Texture>();

    // Ambient lighting.
    eng::Material ambient_light_mat = {};

    // Active camera.
    std::shared_ptr<eng::Camera> active_camera = {};

    // Player camera.
    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};

    eng::RenderPass ambient_light_pass;
    
    void init() override {
        { // Shader compiler.
            shader_compiler.root = local::shader_folder;
        }
        { // Fullscreen primitive.
            auto& p = *fullscreen_prim;
            p.vertex_array = agl::vertex_array();
            p.primitive_count = agl::Count<GLsizei>(6);
        }
        { // Default sampler.
            default_sampler = create(agl::sampler_tag);
            mag_filter(default_sampler, GL_LINEAR);
            min_filter(default_sampler, GL_LINEAR);
        }
        { // Default textures.
            *default_material = gltf::g_buffer_material();
            { // Albedo.
                auto& t = *default_albedo_tex = eng::default_albedo_texture(agl::vec3(1.f));
                t.sampler = default_sampler;
            }
            { // Emissive map.
                auto& t = *default_emissive_tex = eng::default_emissive_texture(agl::vec3(0.f));
                t.sampler = default_sampler;
            }
            { // Normal map.
                auto& t = *default_normal_map_tex = eng::default_normal_texture(agl::vec3(0.f, 0.f, 1.f));
                t.sampler = default_sampler;
            }
            { // Occlusion map.
                auto& t = *default_occlusion_tex = eng::default_occlusion_texture(1.f);
                t.sampler = default_sampler;
            }
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
        
        // { // Changes primitives linked program.
        //     for(auto& m : scene.meshes | ranges::views::values | ranges::views::indirect)
        //     for(auto& p : m.primitives | ranges::views::indirect) {
        //         p.material->program = *ambient_light_pass.program;
        //         eng::bind(p, *p.material);
        //     }
        // }

        // { // Add meshes to ambient lighting pass.
        //     for(auto& m : scene.meshes | ranges::views::values | ranges::views::indirect) {
        //         add(ambient_light_pass, m);
        //     }
        // }

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

        auto inv_v = transform(view);
        auto v = inverse(inv_v);

        auto vp = transform(*active_camera) * v;

        { // Ambient light pass.
            for(auto& m : scene.meshes | ranges::views::values | ranges::views::indirect)
            for(auto& p : m.primitives | ranges::views::indirect) {
                    bind(p);
                    uniform(p.material->program, "mvp_transform", vp);
                    // auto normal_transform = transpose(inverse(mv));
                    // uniform(p.material->program.program, "normal_transform", normal_transform);
                    eng::render(p);
                    unbind(p);
            }
        }

        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("Open")) {
                    for(auto& f : files) {
                        if(ImGui::MenuItem(f.c_str())) {
                            load_model(*this, f);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
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


            for(auto& t : m.textures | ranges::views::values | ranges::views::indirect) {
                if(!t.sampler) {
                    t.sampler = program.default_sampler;
                }
            }

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
