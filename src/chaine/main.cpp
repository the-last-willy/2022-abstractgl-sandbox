// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

#include "all.hpp"

#include <local/all.hpp>
#include "program/all.hpp"

#include <agl/all.hpp>
#include <agl/format/off/all.hpp>

// External libraries.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard library.

#include <iostream>

//

using namespace chaine;

struct RenderSettings {
    bool show_edges = true;
    bool show_triangles = true;
    bool show_vertices = true;
};

struct App : Program {
    eng::ShaderCompiler shader_compiler = {};

    std::shared_ptr<eng::Mesh> drawable_mesh = nullptr;

    tlw::View view = {};
    eng::PerspectiveProjection projection = {};

    eng::RenderPass render_pass = {};

    void init() override {
        shader_compiler.root = local::src_folder;

        { // Render pass.
            render_pass.program = std::make_shared<eng::Program>();
            load(*render_pass.program, shader_compiler, {
                {
                    agl::vertex_shader_tag,
                    "chaine/shader/solid.vs"
                },
                {
                    agl::fragment_shader_tag,
                    "chaine/shader/solid.fs"
                }
            });
            render_pass.program->capabilities.push_back({ agl::Capability::cull_face, []() {
                glCullFace(GL_BACK); }});
            render_pass.program->capabilities.push_back({ agl::Capability::depth_test, []() {
                glDepthFunc(GL_LESS); }});
        }

        auto off = format::off::read(local::root_folder + "/data/queen.off");
        std::cout << std::endl;

        FaceVertexMesh fvmesh;


        auto mesh = TriangleMesh();
        {
            mesh.triangle_indices = off.triangle_indices;
            mesh.vertex_positions = off.vertex_positions;
        }

        {
            drawable_mesh = vertex_mesh(mesh);
            for(auto& p : drawable_mesh->primitives) {
                p->material = std::make_shared<eng::Material>();
            }
            add(render_pass, *drawable_mesh);
        }

        projection.aspect_ratio = 16.f / 9.f;
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
                view.position = view.position - direction / 100.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_D)) {
                auto direction = (rotation(view) * agl::rotation_y(agl::pi / 2.f))[2].xyz();
                view.position = view.position + direction / 100.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_S)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position - direction / 100.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_W)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position + direction / 100.f;
            }
        }
    }

    void render() override {
        glClearDepthf(1.f);
        glClear(GL_DEPTH_BUFFER_BIT);
        bind(*render_pass.program);
        for(std::size_t i = 0; i < size(render_pass.primitives); ++i) {
            auto& p = *render_pass.primitives[i];
            auto& va = render_pass.vertex_arrays[i];
            bind(*p.material, *render_pass.program);
            bind(va);
            uniform(*render_pass.program, "mvp", transform(projection) * inverse(transform(view)));
            eng::render(p, va);
        }
        unbind(*render_pass.program);
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
