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

struct PointLight {
    agl::Vec3 position = {};
};

struct GltfProgram : Program {
    eng::Database database = {};

    // glTF file.
    format::gltf2::Content scene = {};

    // G buffer.
    eng::Framebuffer g_buffer = {};
    agl::Texture albedo_texture = {};
    agl::Texture depth_texture = {};
    agl::Texture emissive_texture = {};
    agl::Texture metallic_roughness_texture = {};
    agl::Texture normal_texture = {};
    agl::Texture position_texture = {};

    // HDR.
    agl::Texture hdr_tex = {};
    agl::Framebuffer hdr_fb = {}; 

    // Skybox.
    eng::Material skybox_mat = {};

    // Tone mapping.
    eng::Material tone_mapping_mat = {};

    // Shadow map.
    eng::Framebuffer shadow_map_fb = {};
    eng::Material shadow_map_mat = {};
    int shadow_map_resolution = 4'096;

    // Cube shadow map.
    int cube_shadow_map_res = 4'096;
    agl::Texture cube_shadow_map_tex = {};
    eng::Material cube_shadow_map_mat = {};
    tlw::PerspectiveProjection cube_shadow_map_proj = {};
    std::array<tlw::View, 6> cube_shadow_map_views = {};

    // Ambient lighting.
    eng::Material ambient_light_mat = {};

    // Directional light.
    DirectionalLight dir_light = {};
    eng::Material dir_light_mat = {};

    // Spot light.
    SpotLight spot_light;
    eng::Material spot_light_mat;

    // Point light.
    PointLight point_light = {};
    eng::Material point_light_mat = {};

    // Player camera.
    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};
    
    void init() override {
        { // Database defaults.
            database.default_material = gltf::g_buffer_material();
            { // Albedo map.
                database.default_albedo_map = agl::create(agl::TextureTarget::_2d);
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
                database.default_normal_map = agl::create(agl::TextureTarget::_2d);
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
            // "D:/data/sample/gltf2/sponza/Sponza/glTF/Sponza.gltf"
            "D:/data/sample/gltf2/damaged_helmet/DamagedHelmet/glTF/DamagedHelmet.gltf"
            // "D:/data/sample/gltf2/Buggy/glTF/Buggy.gltf"
            // "D:/data/sample/gltf2/ReciprocatingSaw/glTF/ReciprocatingSaw.gltf"
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

        scene = format::gltf2::fill(model);

        { // Precomputing model transforms.

        }

        { // Normalizing materials.
            for(auto& mesh : scene.meshes | std::views::values | common::views::indirect)
            for(auto& primitive : mesh.primitives | common::views::indirect) {
                auto& m = primitive.material;
                m.program.program = database.default_material.program.program;

                if(!m.textures.contains("baseColorTexture")) {
                    m.textures["baseColorTexture"] = database.default_albedo_map;
                }
                if(!m.textures.contains("normalTexture")) {
                    m.textures["normalTexture"] = database.default_normal_map;
                }
            }
        }

        { // Vertex attribute plumbing.
            for(auto& mesh : scene.meshes | std::views::values | common::views::indirect)
            for(auto& primitive : mesh.primitives | common::views::indirect) {
                bind(primitive, primitive.material);
            }
        }

        { // GBuffer.
            g_buffer.opengl = agl::create(agl::framebuffer_tag);
            { // Albedo texture.
                albedo_texture = agl::create(agl::TextureTarget::_2d);
                mag_filter(albedo_texture, GL_LINEAR);
                min_filter(albedo_texture, GL_LINEAR);
                storage(
                    albedo_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::ColorAttachment(0),
                    albedo_texture);
            }
            { // Depth texture.
                depth_texture = agl::create(agl::TextureTarget::_2d);
                storage(
                    depth_texture, GL_DEPTH_COMPONENT32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::depth_tag,
                    depth_texture);
            }
            { // Emissive texture.
                emissive_texture = agl::create(agl::TextureTarget::_2d);
                storage(
                    emissive_texture, GL_R11F_G11F_B10F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::ColorAttachment(1),
                    emissive_texture);
            }
            { // Metallic roughness texture.
                metallic_roughness_texture = agl::create(agl::TextureTarget::_2d);
                storage(
                    metallic_roughness_texture, GL_R11F_G11F_B10F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::ColorAttachment(2),
                    metallic_roughness_texture);
            }
            { // Normal texture.
                normal_texture = agl::create(agl::TextureTarget::_2d);
                mag_filter(normal_texture, GL_LINEAR);
                min_filter(normal_texture, GL_LINEAR);
                storage(
                    normal_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::ColorAttachment(3),
                    normal_texture);
            }
            { // Position texture. 
                position_texture = agl::create(agl::TextureTarget::_2d);
                mag_filter(position_texture, GL_LINEAR);
                min_filter(position_texture, GL_LINEAR);
                storage(
                    position_texture, GL_RGB32F,
                    agl::Width(window.width()), agl::Height(window.height()));
                texture(g_buffer.opengl,
                    agl::ColorAttachment(4),
                    position_texture);
            }
            auto fbs = std::array<agl::FramebufferDrawBuffer, 5>{
                agl::ColorAttachment(0),
                agl::ColorAttachment(1),
                agl::ColorAttachment(2),
                agl::ColorAttachment(3),
                agl::ColorAttachment(4),};
            draw_buffers(g_buffer.opengl, std::span(fbs));
        }
        
        { // Shadow map.
            shadow_map_mat = gltf::shadow_mapping_material();
            shadow_map_mat.on_bind = [=]() {
                glViewport(0, 0, shadow_map_resolution, shadow_map_resolution);
            };

            shadow_map_fb.opengl = create(agl::framebuffer_tag);
            shadow_map_fb.on_clear = [](const agl::Framebuffer& f) {
                clear(f, agl::depth_tag, 1.f); };
        }

        { // Cube shadow map.
            cube_shadow_map_tex = gltf::cube_shadow_mapping_texture(cube_shadow_map_res);
            cube_shadow_map_mat = gltf::cubic_shadow_mapping_material();
            cube_shadow_map_mat.on_bind = [=]() {
                glViewport(0, 0, cube_shadow_map_res, cube_shadow_map_res);
            };

            cube_shadow_map_proj.aspect_ratio = 1.f;

            // X+
            cube_shadow_map_views[0].yaw = agl::pi / 2.f;
            // X-
            cube_shadow_map_views[1].yaw = -agl::pi / 2.f;
            // Y+
            cube_shadow_map_views[2].pitch = agl::pi / 2.f;
            // Y-
            cube_shadow_map_views[3].pitch = -agl::pi / 2.f;
            // Z+
            cube_shadow_map_views[4].yaw = 0.f;
            // Z-
            cube_shadow_map_views[5].yaw = agl::pi;
        }

        { // Skybox.
            skybox_mat = gltf::skybox_material();
            skybox_mat.textures["cube_map"] = cube_shadow_map_tex;
        }

        { // Ambient light.
            ambient_light_mat = gltf::ambient_lighting_material();
            ambient_light_mat.textures["albedo_texture"] = albedo_texture;
            ambient_light_mat.textures["emissive_texture"] = emissive_texture;
            ambient_light_mat.textures["normal_texture"] = normal_texture;
            ambient_light_mat.textures["position_texture"] = position_texture;
        }
        { // Directional light.
            dir_light.shadow_map = gltf::shadow_mapping_texture(shadow_map_resolution);
            dir_light_mat = gltf::directional_lighting_material();
            dir_light_mat.textures["albedo_texture"] = albedo_texture;
            dir_light_mat.textures["normal_texture"] = normal_texture;
            dir_light_mat.textures["position_texture"] = position_texture;
            dir_light_mat.textures["shadow_map"] = dir_light.shadow_map;
        }
        { // Spot light.
            spot_light.shadow_map = gltf::shadow_mapping_texture(shadow_map_resolution);
            spot_light_mat = gltf::spot_lighting_material();
            spot_light_mat.textures["albedo_texture"] = albedo_texture;
            spot_light_mat.textures["normal_texture"] = normal_texture;
            spot_light_mat.textures["position_texture"] = position_texture;
            spot_light_mat.textures["shadow_map"] = spot_light.shadow_map;
        }

        { // Point light.
            point_light_mat = gltf::point_lighting_material();
            point_light_mat.textures["albedo_texture"] = albedo_texture;
            point_light_mat.textures["normal_texture"] = normal_texture;
            point_light_mat.textures["position_texture"] = position_texture;
            point_light_mat.textures["shadow_map"] = cube_shadow_map_tex;
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
            texture(hdr_fb, agl::ColorAttachment(0), hdr_tex);
            draw_buffer(hdr_fb, agl::ColorAttachment(0));

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
                view.position = view.position - direction / 10.f;
            }
            if(glfwGetKey(window.window, GLFW_KEY_D)) {
                auto direction = (rotation(view) * agl::rotation_y(-agl::pi / 2.f))[2].xyz();
                view.position = view.position - direction / 10.f;
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

        // Set spot light.
        if(glfwGetKey(window.window, GLFW_KEY_SPACE)) {
            point_light.position = view.position;
        }
    }

    void render() override {
        auto traverse_scene = [](eng::Node& n, auto f) {
            traverse(n, [f](const eng::Mesh& m, const agl::Mat4& transform) {
                for(auto& primitive : m.primitives | common::views::indirect) {
                    f(primitive, transform);
                }
            });
        };

        if constexpr(false) { // Shadow map.
            texture(
                shadow_map_fb.opengl,
                agl::depth_tag,
                dir_light.shadow_map);

            bind(shadow_map_fb);
            clear(shadow_map_fb);

            bind(shadow_map_mat);
            for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                traverse_scene(n, [&](eng::Primitive& primitive, const agl::Mat4& transform) {
                    bind(primitive);
                    auto mvp = dir_light.transform * transform;
                    uniform(shadow_map_mat.program, "mvp", mvp);
                    eng::render(primitive);
                    unbind(primitive);
                });
            }
            unbind(shadow_map_mat);
        }

        if constexpr(false) { // Spot shadow mapping.
            texture(
                shadow_map_fb.opengl,
                agl::depth_tag,
                dir_light.shadow_map);

            bind(shadow_map_fb);
            clear(shadow_map_fb);

            bind(shadow_map_mat);
            for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                traverse_scene(n, [&](eng::Primitive& primitive, const agl::Mat4& transform) {
                    bind(primitive.vertex_array);
                    auto mvp = spot_light.transform * transform;
                    uniform(shadow_map_mat.program, "mvp", mvp);
                    eng::render(primitive);
                    unbind(primitive, shadow_map_mat);
                });
            }
            unbind(shadow_map_mat);
        }

        if constexpr(true) { // Cube shadow mapping.
            bind(shadow_map_fb);

            bind(cube_shadow_map_mat);
            for(int face_i = 0; face_i < 6; ++face_i) {
                texture(
                    shadow_map_fb.opengl,
                    agl::depth_tag,
                    cube_shadow_map_tex,
                    agl::Level(0),
                    face_i);
                glClear(GL_DEPTH_BUFFER_BIT);

                auto vp = transform(cube_shadow_map_proj)
                * inverse(transform(cube_shadow_map_views[face_i]))
                * inverse(agl::translation(point_light.position));

                for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                    traverse_scene(n, [&](eng::Primitive& primitive, const agl::Mat4& transform) {
                        bind(primitive.vertex_array);
                        auto mvp = vp * transform;
                        uniform(cube_shadow_map_mat.program, "far", 1000.f);
                        uniform(cube_shadow_map_mat.program, "mvp", mvp);
                        eng::render(primitive);
                    });
                }
            }
            unbind(cube_shadow_map_mat);
        }

        auto inv_v = transform(view);
        auto v = inverse(inv_v);

        auto vp = transform(projection) * v;
        auto normal_transform = transpose(inv_v);

        { // G buffer.
            glViewport(0, 0, window.width(), window.height());
            bind(g_buffer);
            clear(g_buffer.opengl, agl::depth_tag, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                traverse_scene(n, [&](eng::Primitive& primitive, const agl::Mat4& transform) {
                    bind(primitive);
                    uniform(primitive.material.program, "mv", v * transform);
                    uniform(primitive.material.program, "mvp", vp * transform);
                    uniform(primitive.material.program, "normal_transform", normal_transform);
                    eng::render(primitive);
                    unbind(primitive);
                });
            }
        }

        { // Lights.
            bind(hdr_fb);
            clear(hdr_fb, agl::color_tag, {0.f, 0.f, 0.f, 1.f});

            // Skybox
            if constexpr(false) {
                bind(skybox_mat);
                auto tr = rotation(view);
                uniform(skybox_mat.program, "transform", tr);
                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(skybox_mat);
            }

            if constexpr(true) { // Ambient lighting.
                bind(ambient_light_mat);
                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(ambient_light_mat);
            }

            if constexpr(false) { // Directional lights.
                bind(dir_light_mat);

                auto light_dir = normalize(v * agl::vec4(dir_light.direction, 0.f)).xyz();
                uniform(dir_light_mat.program, "light_direction", light_dir);
                auto light_space_transform = dir_light.transform * transform(view);
                uniform(dir_light_mat.program, "light_space_transform", light_space_transform);
                uniform(dir_light_mat.program, "view_position", view.position);

                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));

                unbind(dir_light_mat);
            }

            if constexpr(false) { // Spot lights.
                bind(dir_light_mat);

                auto light_dir = normalize(v * agl::vec4(spot_light.direction, 0.f)).xyz();
                uniform(dir_light_mat.program, "light_direction", light_dir);
                auto light_space_transform = spot_light.transform * transform(view);
                uniform(dir_light_mat.program, "light_space_transform", light_space_transform);
                uniform(dir_light_mat.program, "view_position", view.position);

                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                
                unbind(dir_light_mat);
            }

            if constexpr(true) { // Point lights.
                bind(point_light_mat);

                auto light_position = v * vec4(point_light.position, 1.f);
                uniform(point_light_mat.program, "light_position", light_position.xyz());
                uniform(point_light_mat.program, "light_transform", inv_v);
                uniform(point_light_mat.program, "view_position", view.position);

                bind(database.empty_vertex_array);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));

                unbind(point_light_mat);
            }
        }

        { // Tone mapping.
            bind(agl::default_framebuffer);
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
