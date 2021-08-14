// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Local headers.

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

struct DirectionalLight {
    agl::Vec3 direction = {};
    agl::Mat4 transform = {};

    agl::Texture shadow_map = {};
};

struct SpotLight {
    agl::Vec3 direction = {};
    agl::Mat4 transform = {};

    agl::Texture shadow_map = {};
};

struct GltfProgram : Program {
    eng::Database database = {};

    // G buffer.
    agl::Framebuffer g_buffer = {};
    agl::Texture albedo_texture = {};
    agl::Texture depth_texture = {};
    agl::Texture normal_texture = {};
    agl::Texture position_texture = {};

    // HDR.
    agl::Texture hdr_tex = {};
    agl::Framebuffer hdr_fb = {}; 

    // Tone mapping.
    eng::Material tone_mapping_mat = {};


    // Shadow map.
    agl::Framebuffer shadow_map_fb = {};
    eng::Material shadow_map_mat = {};
    // agl::Texture shadow_map_tex = {};
    int shadow_map_resolution = 4'096;

    // Ambient lighting.
    eng::Material ambient_light_mat = {};

    // Directional light.
    DirectionalLight dir_light = {};
    // agl::Vec3 directional_light_dir = {};
    eng::Material directional_light_mat = {};
    // agl::Mat4 directional_light_transform = {};

    // Perspective shadow mapping.
    agl::Framebuffer persp_shadow_map_fb = {};
    eng::Material persp_shadow_map_mat = {};

    // Spot light.
    SpotLight spot_light;
    eng::Material spot_light_mat;

    // Player camera.
    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};
    
    void init() override {
        { // Database defaults.
            database.default_material = gltf::g_buffer_material();
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
        
        { // Shadow map.
            shadow_map_mat = gltf::shadow_mapping_material();
            shadow_map_mat.on_bind = [=]() {
                glViewport(0, 0, shadow_map_resolution, shadow_map_resolution);
            };

            shadow_map_fb = create(agl::framebuffer_tag);
        }
        { // Perspective shadow mapping.
            persp_shadow_map_mat = gltf::perspective_shadow_mapping_material();
            persp_shadow_map_mat.on_bind = [=]() {
                glViewport(0, 0, shadow_map_resolution, shadow_map_resolution);
            };

            persp_shadow_map_fb = create(agl::framebuffer_tag);
        }
        { // Ambient light.
            ambient_light_mat = gltf::ambient_lighting_material();
            ambient_light_mat.textures.emplace(
                "albedo_texture", albedo_texture);
            ambient_light_mat.textures.emplace(
                "normal_texture", normal_texture);
            ambient_light_mat.textures.emplace(
                "position_texture", position_texture);
        }
        { // Directional light.
            dir_light.shadow_map = gltf::shadow_mapping_texture(
                shadow_map_resolution);

            directional_light_mat = gltf::directional_lighting_material();
            directional_light_mat.textures.emplace(
                "albedo_texture", albedo_texture);
            directional_light_mat.textures.emplace(
                "normal_texture", normal_texture);
            directional_light_mat.textures.emplace(
                "position_texture", position_texture);
            directional_light_mat.textures.emplace(
                "shadow_map", dir_light.shadow_map);
        }
        { // Spot light.
            spot_light.shadow_map = gltf::shadow_mapping_texture(
                shadow_map_resolution);

            spot_light_mat = gltf::spot_lighting_material();
            spot_light_mat.textures.emplace(
                "albedo_texture", albedo_texture);
            spot_light_mat.textures.emplace(
                "normal_texture", normal_texture);
            spot_light_mat.textures.emplace(
                "position_texture", position_texture);
            spot_light_mat.textures.emplace(
                "shadow_map", spot_light.shadow_map);
        }

        { // HDR.
            hdr_tex = create(agl::TextureTarget::_2d);
            mag_filter(position_texture, GL_NEAREST);
            min_filter(position_texture, GL_NEAREST);
            storage(
                hdr_tex,
                GL_RGB16F,
                agl::Width(window.width()), agl::Height(window.height()));

            hdr_fb = create(agl::framebuffer_tag);
            texture(hdr_fb, agl::color_attachment(0), hdr_tex);
            draw_buffer(hdr_fb, agl::FramebufferBuffer::color0);

            tone_mapping_mat = gltf::tone_mapping_material();
            tone_mapping_mat.textures["hdr_map"] = hdr_tex;
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

        // Set directional light.
        if(glfwGetKey(window.window, GLFW_KEY_SPACE)) {
            eng::OrthographicProjection op = {};
            op.zoom = 1.f / 500.f;
            op.near_ = 0.f;
            op.far_ = 1000.f;
            dir_light.direction = agl::normalize(rotation(view)[2].xyz());
            dir_light.transform = transform(op) * inverse(transform(view));
        }

        // Set spot light.
        if(glfwGetKey(window.window, GLFW_KEY_SPACE)) {
            tlw::PerspectiveProjection pp = {};
            pp.near_ = 0.1f;
            pp.far_ = 1000.f;
            spot_light.direction = agl::normalize(rotation(view)[2].xyz());
            spot_light.transform = transform(pp) * inverse(transform(view));
        }
    }

    void render() override {
        auto model = agl::scaling3(0.1f);

        { // Shadow map.
            texture(
                shadow_map_fb,
                agl::TextureAttachment::depth,
                dir_light.shadow_map);

            bind(agl::FramebufferTarget::framebuffer, shadow_map_fb);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            auto mvp = dir_light.transform * model;

            bind(shadow_map_mat);
            for(auto& p : database.primitives) {
                bind(p.vertex_array);

                uniform(shadow_map_mat.program, "mvp", mvp);

                eng::render(p);
            }
            unbind(shadow_map_mat);
        }

        { // Perspective shadow mapping.
            texture(
                persp_shadow_map_fb,
                agl::TextureAttachment::depth,
                spot_light.shadow_map);

            bind(agl::FramebufferTarget::framebuffer, persp_shadow_map_fb);
            glClear(GL_DEPTH_BUFFER_BIT);

            auto mvp = spot_light.transform * model;

            bind(persp_shadow_map_mat);
            for(auto& p : database.primitives) {
                bind(p.vertex_array);

                uniform(persp_shadow_map_mat.program, "mvp", mvp);

                uniform(persp_shadow_map_mat.program, "near", 0.f);
                uniform(persp_shadow_map_mat.program, "far", 1000.f);

                eng::render(p);
            }
            unbind(persp_shadow_map_mat);
        }

        auto inv_v = transform(view);
        auto v = inverse(inv_v) * model;

        auto mvp = transform(projection) * v;
        auto normal_transform = transpose(inv_v);

        { // G buffer.
            glViewport(0, 0, window.width(), window.height());
            bind(agl::FramebufferTarget::framebuffer, g_buffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        }

        { // Lights.
            bind(agl::FramebufferTarget::framebuffer, hdr_fb);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            { // Ambient lighting.
                bind(ambient_light_mat);
                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));

                unbind(ambient_light_mat);
            }

            { // Directional lights.
                bind(directional_light_mat);
                {
                    auto light_dir = normalize(v * agl::vec4(dir_light.direction, 0.f)).xyz();
                    uniform(
                        directional_light_mat.program,
                        "light_direction",
                        light_dir);
                    auto light_space_transform = dir_light.transform * transform(view);
                    uniform(
                        directional_light_mat.program,
                        "light_space_transform",
                        light_space_transform);
                    uniform(
                        directional_light_mat.program,
                        "view_position",
                        view.position);
                }

                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));

                unbind(directional_light_mat);
            }

            // { // Spot lights.
            //     bind(spot_light_mat);
            //     {
            //         auto light_dir = normalize(v * agl::vec4(spot_light.direction, 0.f)).xyz();
            //         uniform(spot_light_mat.program, "light_direction", light_dir);
            //         auto light_space_transform = spot_light.transform * transform(view);
            //         uniform(spot_light_mat.program, "light_space_transform", light_space_transform);
            //         uniform(spot_light_mat.program, "view_position", view.position);
            //     }

            //     bind(database.empty_vertex_array);
            //     draw_arrays(
            //         agl::DrawMode::triangles,
            //         agl::Offset<GLint>(0),
            //         agl::Count<GLsizei>(6));

            //     unbind(spot_light_mat);
            // }
        }

        { // Tone mapping.
            unbind(agl::FramebufferTarget::framebuffer);
            bind(tone_mapping_mat);

            bind(database.empty_vertex_array);
            draw_arrays(
                agl::DrawMode::triangles,
                agl::Offset<GLint>(0),
                agl::Count<GLsizei>(6));

            unbind(tone_mapping_mat);
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
