// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Local headers.

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

struct GltfProgram : Program {
    eng::Database database = {};

    agl::Framebuffer g_buffer = {};
    agl::Texture albedo_texture = {};
    agl::Texture depth_texture = {};
    agl::Texture normal_texture = {};
    agl::Texture position_texture = {};

    agl::Framebuffer shadow_map_fb = {};
    eng::Material shadow_map_mat = {};
    agl::Texture shadow_map_tex = {};
    int shadow_map_resolution = 1'024;

    agl::Mat4 directional_light_transform = {};
    
    eng::Material lighting_material = {};

    eng::Material ambient_lighting_material = {};
    eng::Material directional_lighting_material = {};

    eng::Material debug_material = {};

    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};
    
    void init() override {
        { // Database defaults.
            { // Material.
                load(database.default_material.program, {
                    {
                        agl::ShaderType::vertex,
                        file(tlw::root + "src/shader/gltf/deferred/g_buffer.vs"
                        )
                    },
                    {
                        agl::ShaderType::fragment,
                        file(tlw::root + "src/shader/gltf/deferred/g_buffer.fs"
                        )
                    }});
                database.default_material.program.capabilities = {
                    agl::Capability::depth_test};
                database.default_material.on_enter = []() {
                    glDepthFunc(GL_LESS);
                };
            }
            { // Albedo map.
                database.default_albedo_map = create(agl::TextureTarget::_2d);
                storage(
                    database.default_albedo_map,
                    GL_RGB32F, agl::Width(1), agl::Height(1));
                auto rgb = std::array{agl::vec3(0.5f, 0.5f, 0.5f)};
                image(
                    database.default_albedo_map,
                    agl::Level(0),
                    0, 0, agl::Width(0), agl::Height(0),
                    GL_RGB, GL_FLOAT,
                    as_bytes(std::span(rgb)));
            }
            { // Normal map.
                database.default_normal_map = create(agl::TextureTarget::_2d);
                storage(
                    database.default_normal_map,
                    GL_RGB32F, agl::Width(1), agl::Height(1));
                auto normal = std::array{agl::vec3(0.f, 0.f, 1.f)};
                image(
                    database.default_normal_map,
                    agl::Level(0),
                    0, 0, agl::Width(0), agl::Height(0),
                    GL_RGB, GL_FLOAT,
                    as_bytes(std::span(normal)));
            }
        }

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(
            &model, &err, &warn, 
            "D:/data/sample/gltf2/sponza/Sponza/glTF/Sponza.gltf"
            // "D:/data/sample/gltf2/box/Box/glTF/Box.gltf"
            // "D:/data/sample/gltf2/box_textured/BoxTextured/glTF/BoxTextured.gltf"
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

        format::gltf2::fill(database, model);

        { // GBuffer.
            g_buffer = create(agl::framebuffer_tag);
            { // Albedo texture.
                albedo_texture = create(agl::TextureTarget::_2d);
                mag_filter(albedo_texture, GL_LINEAR);
                min_filter(albedo_texture, GL_LINEAR);
                storage(
                    albedo_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer,
                    agl::color_attachment(0),
                    albedo_texture);
            }
            { // Depth texture.
                depth_texture = create(agl::TextureTarget::_2d);
                storage(
                    depth_texture, GL_DEPTH_COMPONENT32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer,
                    agl::TextureAttachment::depth,
                    depth_texture);
            }
            { // Normal texture.
                normal_texture = create(agl::TextureTarget::_2d);
                mag_filter(normal_texture, GL_LINEAR);
                min_filter(normal_texture, GL_LINEAR);
                storage(
                    normal_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer,
                    agl::color_attachment(1),
                    normal_texture);
            }
            { // Position texture. 
                position_texture = create(agl::TextureTarget::_2d);
                mag_filter(position_texture, GL_LINEAR);
                min_filter(position_texture, GL_LINEAR);
                storage(
                    position_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer,
                    agl::color_attachment(2),
                    position_texture);
            }
            auto fbs = std::array{
                agl::FramebufferBuffer::color0,
                agl::FramebufferBuffer::color1,
                agl::FramebufferBuffer::color2};
            draw_buffers(g_buffer, std::span(fbs));
        }

        { // Loading ambient lighting material.
            load(ambient_lighting_material.program, {
                {
                    agl::ShaderType::vertex,
                    file(tlw::root + "src/shader/gltf/deferred/ambient.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(tlw::root + "src/shader/gltf/deferred/ambient.fs")
                }});
            ambient_lighting_material.program.capabilities = {};
            ambient_lighting_material.on_enter = []() {};
            ambient_lighting_material.textures.push_back(
                {"albedo_texture", albedo_texture});
            ambient_lighting_material.textures.push_back(
                {"normal_texture", normal_texture});
            ambient_lighting_material.textures.push_back(
                {"position_texture", position_texture});
        }
        { // Shadow maps.
            shadow_map_tex = create(agl::TextureTarget::_2d);
            mag_filter(shadow_map_tex, GL_NEAREST);
            min_filter(shadow_map_tex, GL_NEAREST);
            parameter(shadow_map_tex, agl::TextureParameter::wrap_s, GL_REPEAT); 
            parameter(shadow_map_tex, agl::TextureParameter::wrap_t, GL_REPEAT);  
            storage(
                shadow_map_tex,
                GL_DEPTH_COMPONENT32F,
                agl::Width(shadow_map_resolution),
                agl::Height(shadow_map_resolution));

            shadow_map_mat.on_enter = [=, f = shadow_map_mat.on_enter]() {
                f();
                glCullFace(GL_FRONT);
                glDepthFunc(GL_LESS);
                glViewport(0, 0, shadow_map_resolution, shadow_map_resolution);
            };
            load(shadow_map_mat.program, {
                {
                    agl::ShaderType::vertex, 
                    file(tlw::root + "src/shader/gltf/shadow_map.vs")
                },
                {
                    agl::ShaderType::fragment, 
                    file(tlw::root + "src/shader/gltf/shadow_map.fs")
                }});
            shadow_map_mat.program.capabilities = {
                agl::Capability::cull_face,
                agl::Capability::depth_test};

            shadow_map_fb = create(agl::framebuffer_tag);
            // glDrawBuffer(GL_NONE);
            // glReadBuffer(GL_NONE);
            texture(shadow_map_fb,
                agl::TextureAttachment::depth,
                shadow_map_tex);
        }
        { // Loading directional lighting material.
            load(directional_lighting_material.program, {
                {
                    agl::ShaderType::vertex,
                    file(tlw::root + "src/shader/gltf/deferred/directional_lighting.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(tlw::root + "src/shader/gltf/deferred/directional_lighting.fs")
                }});
            directional_lighting_material.program.capabilities = {
                    agl::Capability::blend};
            directional_lighting_material.on_enter = []() {
                glBlendFunc(GL_ONE, GL_ONE);
            };
            directional_lighting_material.textures.push_back(
                {"albedo_texture", albedo_texture});
            directional_lighting_material.textures.push_back(
                {"normal_texture", normal_texture});
            directional_lighting_material.textures.push_back(
                {"position_texture", position_texture});
            directional_lighting_material.textures.push_back(
                {"shadow_map", shadow_map_tex});
        }

        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        { // Camera.
            
        }
        
    }

    void update() override {
        {
            agl::Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw += d[0] / 500.f;
            view.pitch += d[1] / 500.f;

            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window.window, GLFW_KEY_A)) {
                auto direction = (rotation(view) * agl::rotation_y(agl::pi / 2.f))[2].xyz();
                view.position = view.position - direction;
            }
            if(glfwGetKey(window.window, GLFW_KEY_D)) {
                auto direction = (rotation(view) * agl::rotation_y(-agl::pi / 2.f))[2].xyz();
                view.position = view.position - direction;
            }
            if(glfwGetKey(window.window, GLFW_KEY_S)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position - direction;
            }
            if(glfwGetKey(window.window, GLFW_KEY_W)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position + direction;
            }
        }
        if(glfwGetKey(window.window, GLFW_KEY_SPACE)) { // Directional light.
            eng::OrthographicProjection op = {};
            op.zoom = 1.f / 1000.f;
            op.near_ = 0.f;
            op.far_ = 1000.f;
            directional_light_transform = transform(op) * inverse(transform(view));
        }
    }

    void render() override {
        auto model = agl::scaling3(0.1f);

        { // Shadow map.
            bind(agl::FramebufferTarget::framebuffer, shadow_map_fb);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            auto mvp = directional_light_transform * model;

            bind(shadow_map_mat);
            for(auto& p : database.primitives) {
                bind(p);

                uniform(
                    p.material->program.program,
                    *uniform_location(p.material->program.program, "mvp"),
                    mvp);

                eng::render(p);
            }
            unbind(shadow_map_mat);
        }

        glViewport(0, 0, window.width(), window.height());
        
        bind(agl::FramebufferTarget::framebuffer, g_buffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto inv_v = transform(view);
        auto v = inverse(inv_v) * model;

        auto mvp = transform(projection) * v;
        auto normal_transform = transpose(inv_v);

        for(auto& p : database.primitives) {
            bind(p);

            uniform(
                p.material->program.program,
                *uniform_location(p.material->program.program, "mv"),
                v);
            uniform(
                p.material->program.program,
                *uniform_location(p.material->program.program, "mvp"),
                mvp);
            uniform(
                p.material->program.program,
                *uniform_location(p.material->program.program, "normal_transform"),
                normal_transform);

            eng::render(p);

            unbind(p);
        }

        unbind(agl::FramebufferTarget::framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // Ambient lighting.
            bind(ambient_lighting_material);
            bind(database.empty_vertex_array);
            draw_arrays(
                agl::DrawMode::triangles,
                agl::Offset<GLint>(0),
                agl::Count<GLsizei>(6));

            unbind(ambient_lighting_material);
        }

        {
            bind(directional_lighting_material);
            {
                auto light_dir = normalize(v * agl::vec4(agl::vec3(-1.f, -1.f, -1.f), 0.f)).xyz();
                uniform(
                    directional_lighting_material.program.program,
                    *uniform_location(
                        directional_lighting_material.program.program,
                        "light_direction"),
                    light_dir);
                auto light_space_transform = directional_light_transform * transform(view);
                uniform(
                    directional_lighting_material.program.program,
                    *uniform_location(
                        directional_lighting_material.program.program,
                        "light_space_transform"),
                    light_space_transform);
                uniform(
                    directional_lighting_material.program.program,
                    *uniform_location(
                        directional_lighting_material.program.program,
                        "view_position"),
                    view.position);
            }

            bind(database.empty_vertex_array);
            draw_arrays(
                agl::DrawMode::triangles,
                agl::Offset<GLint>(0),
                agl::Count<GLsizei>(6));

            unbind(directional_lighting_material);
        }
    }
};

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
