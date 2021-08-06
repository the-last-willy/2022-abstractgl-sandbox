#include <glad/glad.h>

#include "agl/all.hpp"

#include "engine/all.hpp"
#include "mesh/all.hpp"
#include "program/all.hpp"
#include "scene/all.hpp"
#include "turtle/all.hpp"
#include "error_callback.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

#include <any>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <span>
#include <typeindex>
#include <vector>

using namespace tlw;

inline
agl::Vec3 hash(agl::Vec3 x) {
    x = agl::vec3(
        agl::dot(x, agl::vec3(127.1f, 311.7f, 74.7f)),
        agl::dot(x, agl::vec3(269.5f, 183.3f, 246.1f)),
        agl::dot(x, agl::vec3(113.5f, 271.9f, 124.6f)));
	return agl::fract(agl::sin(x) * 43758.5453123f);
}

inline
agl::Vec3 voronoi(agl::Vec3 v) {
    auto d = agl::vec3(1., 1., 1.);
    auto i = agl::floor(v);
    for(float j = -1.; j <= 1.; j += 1.f)
    for(float k = -1.; k <= 1.; k += 1.f)
    for(float l = -1.; l <= 1.; l += 1.f) {
        auto seed = i + agl::vec3(j, k, l) + hash(i + agl::vec3(j, k, l));
        auto diff = seed - v;
        if(agl::dot(diff, diff) < agl::dot(d, d)) {
            d = diff;
        }
    }
    return d;
}

struct RockProgram : Program {
    std::deque<Object> objects = {};

    View view = {};
    PerspectiveProjection proj = {};

    void init() override {
        std::srand(static_cast<unsigned>(std::time(0)));

        

        auto baking_material = std::make_shared<Material>(
            make_baking_rock_texture_material());

        auto cube_geometry = std::make_shared<Geometry>(
            make_cube_geometry());

        auto quad_geometry = std::make_shared<Geometry>(
            make_quad_geometry());

        auto subdivided_cube = std::make_shared<Geometry>(
            make_subdivided_cube_geometry(10));

        auto phong_material = std::make_shared<Material>(
            make_monochrome_phong_material());

        auto point_material = std::make_shared<Material>(
            make_point_material());

        auto rock_geometry = std::shared_ptr<Geometry>();
        {
            auto mesh = mesh::FaceVertex();
            for(std::size_t i = 0; i < 12; ++i) {
                auto offset = 3 * i;
                mesh.triangles.push_back(mesh::Triangle{
                    cube_indexes[offset + 0],
                    cube_indexes[offset + 1],
                    cube_indexes[offset + 2],});
            }
            for(std::size_t i = 0; i < 24; ++i) {
                mesh.vertices.push_back(mesh::Vertex{
                    .normal = cube_normals[i],
                    .position = cube_positions[i],
                    .texcoords = cube_texcoords[i],});
            }
            mesh = mesh::subdivided(mesh, 6);

            // Editing.
            for(auto& v : mesh.vertices) {
                v.position = agl::normalize(v.position);
                auto sphere_normal = v.position;

                auto displacement = [sn = sphere_normal](agl::Vec3 v) {
                    return sn * agl::length(voronoi(1.2f * v)) / std::sqrt(2.f);
                };

                v.position = v.position + displacement(v.position);

                auto tg = agl::Vec3();
                if(std::abs(sphere_normal[0]) == 1.f) {
                    tg = agl::cross(sphere_normal, agl::vec3(1.f, 0.f, 0.f));
                } else {
                    tg = agl::cross(sphere_normal, agl::vec3(0.f, 1.f, 0.f));
                }
                tg = agl::normalize(tg);

                // binormal
                auto bn = agl::cross(tg, sphere_normal);

                auto e = 0.15f;
                auto dx = (length(displacement(agl::normalize(sphere_normal + e * tg)))
                    - length(displacement(agl::normalize(sphere_normal - e * tg)))) / e / 2.f;
                auto dy = (length(displacement(agl::normalize(sphere_normal + e * bn)))
                    - length(displacement(agl::normalize(sphere_normal - e * bn)))) / e / 2.f;

                auto normal = agl::normalize(agl::vec3(1.f, -dx, -dy));
                v.normal = agl::normalize(
                    normal[0] * sphere_normal
                    + normal[1] * tg
                    + normal[2] * bn);
            }

            rock_geometry = std::make_shared<Geometry>(mesh::geometry(mesh));
        }

        auto rock_texture = agl::Texture();
        {
            auto size = 1024;
            {
                rock_texture = agl::texture(agl::TextureTarget::_2d);
                agl::storage(rock_texture, 1, GL_RGB8, agl::Width(size), agl::Height(size));
                agl::parameter(rock_texture, agl::TextureParameter::mag_filter, GL_LINEAR);
                agl::parameter(rock_texture, agl::TextureParameter::min_filter, GL_LINEAR);

                agl::parameter(rock_texture, agl::TextureParameter::wrap_s, GL_MIRRORED_REPEAT);
                agl::parameter(rock_texture, agl::TextureParameter::wrap_t, GL_MIRRORED_REPEAT);
            }
            {
                auto fb = agl::framebuffer();
                {
                    agl::texture(fb, agl::TextureAttachment::color0, rock_texture);
                    agl::draw_buffer(fb, agl::FramebufferBuffer::color0);
                }
                {
                    glViewport(0, 0, size, size);
                    agl::bind(agl::FramebufferTarget::framebuffer, fb);

                    auto o = Object();
                    o.geometry = rock_geometry;
                    o.material = baking_material;

                    glClearColor(1.f, 1.f, 1.f, 1.f);
                    glClear(GL_COLOR_BUFFER_BIT);

                    configure(o);
                    ::render(o);
                }
            }
        }

        { // Rock.
            auto object = Object();
            object.geometry = rock_geometry;
            object.material = phong_material;
            object.textures["albedo_texture"] = rock_texture;
            objects.push_back(std::move(object));
        }

        for(auto& o : objects) {
            configure(o);
        }

        view.pitch = 0.5f;
        view.rotation_center[2] = -5;

        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void update() override {
        {
            agl::Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw -= d[0] / 500.f;
            view.pitch -= d[1] / 500.f;

            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window.window, GLFW_KEY_S)) {
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l - 0.1f) / l;
            }
            if(glfwGetKey(window.window, GLFW_KEY_W)) {
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l + 0.1f) / l;
            }
        }
    }

    void render() override {
        agl::bind_default(agl::FramebufferTarget::framebuffer);
        glViewport(0, 0, 1280, 720);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto& object : objects) {
            { // Uniforms.
                auto model = agl::mat4(agl::identity);
                {
                    auto it = object.uniforms.find("model");
                    if(it != end(object.uniforms)) {
                        model = dynamic_cast<const agl::Mat4&>(*it->second);
                    }
                }

                auto view_transform =  inverse(transform(view));

                auto mvp = transform(proj) * view_transform * model;
                object.uniforms["mvp"] = std::make_unique<Uniform<agl::Mat4>>(std::move(mvp));

                auto normal_transform = agl::transpose(inverse(view_transform * model));
                object.uniforms["normal_transform"] = std::make_unique<Uniform<agl::Mat4>>(std::move(normal_transform));
            }
            ::render(object);
        }
    }
};

void throwing_main() {
    auto p = RockProgram();
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
