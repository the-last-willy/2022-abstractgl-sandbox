// Definitions.

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

// Disabled warnings.

#pragma warning(disable : 4005 4996)

// Local headers.

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
        "D:/data/sample/gltf2/sponza/Sponza/glTF/Sponza.gltf",
        "D:/data/sample/gltf2/virtual_city/VC/glTF/VC.gltf",
        "D:/data/sample/gltf2/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf",
        "D:/data/sample/gltf2/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
        
        "D:/data/sample/gltf2/damaged_helmet/DamagedHelmet/glTF/DamagedHelmet.gltf",

        "D:/data/sample/gltf2/AnimatedTriangle/glTF/AnimatedTriangle.gltf",
        "D:/data/sample/gltf2/BoxAnimated/glTF/BoxAnimated.gltf",
        "D:/data/sample/gltf2/MetalRoughSpheresNoTextures/glTF/MetalRoughSpheresNoTextures.gltf",
        
        "D:/data/sample/gltf2/OrientationTest/glTF/OrientationTest.gltf",
        "D:/data/sample/gltf2/boom_box_with_axes/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf",
        "D:/data/sample/gltf2/Buggy/glTF/Buggy.gltf",
        "D:/data/sample/gltf2/ReciprocatingSaw/glTF/ReciprocatingSaw.gltf",
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

    // G buffer.
    eng::Framebuffer g_buffer = {};
    std::shared_ptr<eng::Texture> depth_texture = std::make_shared<eng::Texture>();

    // HDR.
    std::shared_ptr<eng::Texture> hdr_tex = std::make_shared<eng::Texture>();
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
    std::shared_ptr<eng::Texture> cube_shadow_map_tex = std::make_shared<eng::Texture>();;
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

    // PBR.
    bool fresh_pbr_lighting_mat = true;
    eng::Material pbr_lighting_mat = {};

    // Active camera.
    std::shared_ptr<eng::Camera> active_camera = {};

    // Player camera.
    tlw::PerspectiveProjection projection = {};
    tlw::View view = {};

    // Animation.
    std::vector<eng::AnimationPlayer> animation_players = {};
    std::map<const eng::Node*, agl::Mat4> node_animations = {};
    
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

        load_model(*this, "D:/data/sample/gltf2/BoxAnimated/glTF/BoxAnimated.gltf");

        g_buffer = eng::gbuffer(window.width(), window.height());
        { // Depth texture.
            auto& tex = *depth_texture;
            tex.texture = agl::create(agl::TextureTarget::_2d);
            storage(
                tex.texture, GL_DEPTH_COMPONENT32F,
                agl::Width(window.width()), agl::Height(window.height()));
            texture(g_buffer.opengl,
                agl::depth_tag,
                tex.texture);
        }
        { // Setting color attachments.
            auto frag_data_locations = std::vector<agl::FramebufferDrawBuffer>();
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                auto fdl = frag_data_location(default_material->program.program, name.c_str());
                if(fdl) {
                    frag_data_locations.push_back(agl::ColorAttachment(*fdl));
                    texture(g_buffer.opengl,
                        agl::ColorAttachment(*fdl),
                        texture_ptr->texture);
                }
            }
            draw_buffers(g_buffer.opengl, std::span(frag_data_locations));
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
            cube_shadow_map_tex->texture = gltf::cube_shadow_mapping_texture(cube_shadow_map_res);
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
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                ambient_light_mat.textures[name] = texture_ptr;
            }
        }
        { // Directional light.
            dir_light.shadow_map->texture = gltf::shadow_mapping_texture(shadow_map_resolution);
            dir_light_mat = gltf::directional_lighting_material();
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                dir_light_mat.textures[name] = texture_ptr;
            }
            dir_light_mat.textures["shadow_map"] = dir_light.shadow_map;
        }
        { // Spot light.
            spot_light.shadow_map->texture = gltf::shadow_mapping_texture(shadow_map_resolution);
            spot_light_mat = gltf::spot_lighting_material();
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                spot_light_mat.textures[name] = texture_ptr;
            } 
            spot_light_mat.textures["shadow_map"] = spot_light.shadow_map;
        }

        { // Point light.
            point_light_mat = gltf::point_lighting_material(shader_compiler);
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                point_light_mat.textures[name] = texture_ptr;
            }
            point_light_mat.textures["shadow_map"] = cube_shadow_map_tex;
        }

        { // PBR material.
            try {
                pbr_lighting_mat = gltf::pbr_lighting_material(shader_compiler);
                fresh_pbr_lighting_mat = true;
            } catch(...) {
                fresh_pbr_lighting_mat = false;
            }
            for(auto& [name, texture_ptr] : g_buffer.color_attachments) {
                pbr_lighting_mat.textures[name] = texture_ptr;
             }
        }

        { // HDR.
            auto& tex = *hdr_tex;
            tex.sampler = create(agl::sampler_tag);
            mag_filter(*tex.sampler, GL_NEAREST);
            min_filter(*tex.sampler, GL_NEAREST);
            tex.texture = create(agl::TextureTarget::_2d);
            storage(
                tex.texture,
                GL_RGB16F,
                agl::Width(window.width()), agl::Height(window.height()));

            hdr_fb = create(agl::framebuffer_tag);
            texture(hdr_fb, agl::ColorAttachment(0), tex.texture);
            draw_buffer(hdr_fb, agl::ColorAttachment(0));

            tone_mapping_mat = gltf::tone_mapping_material();
            tone_mapping_mat.textures["hdr_map"] = hdr_tex;
        }

        // glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
        { // Pre-computing camera transforms.
            auto traverse_scene = [](eng::Node& n) {
                traverse_cameras(n, [](eng::Camera& c, const agl::Mat4& transform) {
                    c.transform = transform;
                });
            };
            for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                traverse_scene(n);
            }
        }

        
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
            tlw::OrthographicProjection op = {};
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

        // Reload pbr material.
        if(glfwGetKey(window.window, GLFW_KEY_R)) {
            
            try {
                pbr_lighting_mat.program = gltf::pbr_lighting_material(shader_compiler).program;
                std::cout << "SHADER SUCCESS" << std::endl;
                fresh_pbr_lighting_mat = true;
            } catch(...) {
                fresh_pbr_lighting_mat = false;
            }
        }

        node_animations.clear();
        for(auto& animation_player : animation_players) {
            animation_player.update(dt);
            for(std::size_t i = 0; i < size(animation_player.animation->channels); ++i) {
                auto& channel = *animation_player.animation->channels[i];
                auto interp_info = animation_player.index(i);
                auto& transform = common::find_or_insert(
                    node_animations,
                    channel.target_node.get(),
                    agl::mat4(agl::identity));
                if(channel.target_path == eng::AnimationTargetPath::scale) {
                    transform = agl::mat4(agl::identity);
                } else if(channel.target_path == eng::AnimationTargetPath::rotation) {
                    auto prev_data = at<agl::Vec4>(
                        channel.sampler->output, interp_info.previous);
                    auto prev_q = agl::Quaternion<float>(
                        prev_data[3], agl::vec3(prev_data[0], prev_data[1], prev_data[2]));
                    auto next_data = at<agl::Vec4>(
                        channel.sampler->output, interp_info.next);
                    auto next_q = agl::Quaternion<float>(
                        next_data[3], agl::vec3(next_data[0], next_data[1], next_data[2]));
                    auto s = agl::slerp(prev_q, next_q, interp_info.t);
                    transform = transform * agl::mat4(
                        agl::slerp(prev_q, next_q, interp_info.t));
                } else if(channel.target_path == eng::AnimationTargetPath::translation) {
                    auto prev = agl::translation(at<agl::Vec3>(
                        channel.sampler->output, interp_info.previous));
                    auto next = agl::translation(at<agl::Vec3>(
                        channel.sampler->output, interp_info.next));
                    transform = agl::mix(prev, next, interp_info.t) * transform;
                }
            }
        }
    }

    void traverse_all_nodes(eng::Node& n, auto f, agl::Mat4 parent_transform = mat4(agl::identity)) {
        auto transform = parent_transform * mat4(n.transform);
        auto it = node_animations.find(&n);
        if(it != end(node_animations)) {
            transform = transform * it->second;
        }
        if(n.mesh) {
            f(**n.mesh, transform);
        }
        for(auto& c : n.children) {
            traverse_all_nodes(*c, f, transform);
        }
    }

    void render() override {
        auto traverse_nodes = [&](eng::Node& n, auto f, agl::Mat4 parent_transform = mat4(agl::identity)) {
            traverse_all_nodes(n, f, parent_transform);
        };

        auto traverse_scene = [&](eng::Node& n, auto f) {
            traverse_nodes(n, [&, f](const eng::Mesh& m, const agl::Mat4& transform) {
                for(auto& primitive : m.primitives | common::views::indirect) {
                    f(primitive, transform);
                }
            });
        };

        if constexpr(false) { // Shadow map.
            texture(
                shadow_map_fb.opengl,
                agl::depth_tag,
                dir_light.shadow_map->texture);

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
                dir_light.shadow_map->texture);

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
                    cube_shadow_map_tex->texture,
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

        auto vp = transform(*active_camera) * v;

        { // G buffer.
            glViewport(0, 0, window.width(), window.height());
            bind(g_buffer);
            clear(g_buffer.opengl, agl::depth_tag, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            for(auto& n : begin(scene.scenes)->second->nodes | common::views::indirect) {
                traverse_scene(n, [&](eng::Primitive& primitive, const agl::Mat4& transform) {
                    if(primitive.material) {
                        auto& m = *primitive.material;
                        bind(primitive);
                        auto mv = v * transform;
                        uniform(m.program, "mv", mv);
                        uniform(m.program, "mvp", vp * transform);
                        auto normal_transform = transpose(inverse(mv));
                        uniform(m.program, "normal_transform", normal_transform);
                        eng::render(primitive);
                        unbind(primitive);
                    }
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
                bind(*fullscreen_prim);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(*fullscreen_prim);
                unbind(skybox_mat);
            }

            if constexpr(true) { // Ambient lighting.
                bind(ambient_light_mat);
                bind(*fullscreen_prim);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(*fullscreen_prim);
                unbind(ambient_light_mat);
            }

            if constexpr(false) { // Directional lights.
                bind(dir_light_mat);

                auto light_dir = normalize(v * agl::vec4(dir_light.direction, 0.f)).xyz();
                uniform(dir_light_mat.program, "light_direction", light_dir);
                auto light_space_transform = dir_light.transform * transform(view);
                uniform(dir_light_mat.program, "light_space_transform", light_space_transform);
                uniform(dir_light_mat.program, "view_position", view.position);

                bind(*fullscreen_prim);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(*fullscreen_prim);

                unbind(dir_light_mat);
            }

            if constexpr(false) { // Spot lights.
                bind(dir_light_mat);

                auto light_dir = normalize(v * agl::vec4(spot_light.direction, 0.f)).xyz();
                uniform(dir_light_mat.program, "light_direction", light_dir);
                auto light_space_transform = spot_light.transform * transform(view);
                uniform(dir_light_mat.program, "light_space_transform", light_space_transform);
                uniform(dir_light_mat.program, "view_position", view.position);

                bind(*fullscreen_prim);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(*fullscreen_prim);
                
                unbind(dir_light_mat);
            }

            if constexpr(true) { // Point lights.
                bind(point_light_mat);

                auto light_position = v * vec4(point_light.position, 1.f);
                uniform(point_light_mat.program, "light_position", light_position.xyz());
                uniform(point_light_mat.program, "light_transform", inv_v);
                uniform(point_light_mat.program, "view_position", view.position);

                bind(*fullscreen_prim);
                draw_arrays(
                    agl::DrawMode::triangles,
                    agl::Offset<GLint>(0),
                    agl::Count<GLsizei>(6));
                unbind(*fullscreen_prim);

                unbind(point_light_mat);
            }

            // PBR.
            if constexpr(false) {
                // if(fresh_pbr_lighting_mat) {
                    bind(pbr_lighting_mat);

                    auto light_position = v * vec4(point_light.position, 1.f);
                    uniform(pbr_lighting_mat.program, "light_position", light_position.xyz());

                    bind(*fullscreen_prim);
                    draw_arrays(
                        agl::DrawMode::triangles,
                        agl::Offset<GLint>(0),
                        agl::Count<GLsizei>(6));
                    unbind(*fullscreen_prim);

                    unbind(pbr_lighting_mat);
                // }
            }
        }

        { // Tone mapping.
            bind(agl::default_framebuffer);
            bind(tone_mapping_mat);

            bind(*fullscreen_prim);
            draw_arrays(
                agl::DrawMode::triangles,
                agl::Offset<GLint>(0),
                agl::Count<GLsizei>(6));
            unbind(*fullscreen_prim);

            unbind(tone_mapping_mat);
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
            if(ImGui::BeginMenu("Animations")) {
                for(std::size_t i = 0; i < size(scene.animations); ++i) {
                    if(ImGui::MenuItem(std::to_string(i).c_str())) {
                        animation_players.clear();
                        auto& ap = animation_players.emplace_back();
                        ap.animation = scene.animations[i];
                    }
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
        for(auto& c : program.scene.cameras | std::views::values | common::views::indirect) {
            if(auto pp = std::get_if<eng::PerspectiveProjection>(&c.projection)) {
                pp->aspect_ratio = 16.f / 9.f;
            }
        }
    }

    { // Normalizing materials.
        for(auto& mesh : program.scene.meshes | std::views::values | common::views::indirect)
        for(auto& primitive : mesh.primitives | common::views::indirect) {
            if(!primitive.material) {
                primitive.material = std::make_shared<eng::Material>();
            }

            auto& m = *primitive.material;


            for(auto& t : m.textures | std::views::values | common::views::indirect) {
                if(!t.sampler) {
                    t.sampler = program.default_sampler;
                }
            }

            m.program.program = program.default_material->program.program;

            if(!m.textures.contains("baseColorTexture")) {
                auto t = eng::Texture();
                m.textures["baseColorTexture"] = program.default_albedo_tex;
            }
            if(!m.textures.contains("emissiveTexture")) {
                m.textures["emissiveTexture"] = program.default_emissive_tex;
            }
            if(!m.textures.contains("normalTexture")) {
                m.textures["normalTexture"] = program.default_normal_map_tex;
            }
            if(!m.textures.contains("occlusionTexture")) {
                m.textures["occlusionTexture"] = program.default_occlusion_tex;
            }
        }
    }

    { // Vertex attribute plumbing.
        for(auto& mesh : program.scene.meshes | std::views::values | common::views::indirect)
        for(auto& primitive : mesh.primitives | common::views::indirect) {
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
