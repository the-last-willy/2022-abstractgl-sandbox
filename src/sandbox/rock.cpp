#include "agl/all.hpp"

#include "engine/all.hpp"
#include "mesh/all.hpp"
#include "scene/all.hpp"
#include "turtle/all.hpp"
#include "error_callback.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

struct Vertex {
    agl::Vec3 color;
    agl::Vec3 normal;
    agl::Vec3 position;
    agl::Vec2 texcoords;
};

inline
auto barycentric_interpolation(
    const Vertex& a,
    const Vertex& b,
    const Vertex& c,
    float tb, float tc)
{
    auto v = Vertex();
    auto ta = 1.f - tb - tc;
    v.color = ta * a.color + tb * b.color + tc * c.color;
    v.normal = ta * a.normal + tb * b.normal + tc * c.normal;
    v.position = ta * a.position + tb * b.position + tc * c.position;
    v.texcoords = ta * a.texcoords + tb * b.texcoords + tc * c.texcoords;
    return v;
}

agl::Vec2 previous_cursor_pos;
agl::Vec2 current_cursor_pos;

static void cursor_position_callback(GLFWwindow*, double x, double y) {
    current_cursor_pos = {float(x), float(y)};
}

static void error_callback(int error, const char* description)
{
    (void) error;
    std::cerr << "Error: " << description << std::endl;
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int throwing_main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    GLFWwindow* window;
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
        window = glfwCreateWindow(1280, 720, "Procedural Terrain Generation", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetKeyCallback(window, key_callback);

        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            previous_cursor_pos = {float(x), float(y)};
            current_cursor_pos = {float(x), float(y)};
        }
    
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            printf("Failed to initialize OpenGL context");
            return -1;
        }

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&error_callback, NULL);
    }

    auto objects = std::deque<Object>();

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

    // { // Quad.
    //     auto object = Object();
    //     object.geometry = quad_geometry;
    //     object.material = phong_material;
    //     object.uniforms["color_rgb"] = agl::vec3(0.8f, 0.8f, 0.8f);
    //     objects.push_back(std::move(object));
    // }

    // { // Cube.
    //     auto object = Object();
    //     object.geometry = cube_geometry;
    //     object.material = phong_material;
    //     object.uniforms["color_rgb"] = agl::vec3(0.8f, 0.8f, 0.0f);
    //     objects.push_back(std::move(object));
    // }


    auto rock_geometry = std::shared_ptr<Geometry>();
    {
        auto mesh = mesh::FaceVertex<Vertex>();
        for(std::size_t i = 0; i < 12; ++i) {
            auto offset = 3 * i;
            mesh.triangles.push_back(mesh::Triangle{
                cube_indexes[offset + 0],
                cube_indexes[offset + 1],
                cube_indexes[offset + 2],});
        }
        for(std::size_t i = 0; i < 24; ++i) {
            mesh.vertices.push_back(Vertex{
                .normal = cube_normals[i],
                .position = cube_positions[i],
                .texcoords = cube_texcoords[i],});
        }
        mesh = mesh::subdivided(mesh, 30);

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

        auto g = Geometry();
        g.primitive_count = agl::Count<GLsizei>(GLsizei(3 * size(mesh.triangles)));
        { // Colors.
            auto b = agl::buffer();
            agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
            auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
            auto idx = 0;
            for(auto& v : mesh.vertices) {
                mapping[idx++] = v.color;
            }
            agl::unmap(b);
            g.attributes["color_rgb"] = AttributeBuffer{
                .buffer = std::move(b),

                .size = agl::Size<GLint>(3),
                .stride = agl::Stride<GLsizei>(12),
                .type = GL_FLOAT,
            };
        }
        { // Indices.
            auto b = agl::buffer();
            agl::storage(b, size(mesh.triangles) * sizeof(mesh::Triangle), GL_MAP_WRITE_BIT);
            auto mapping = agl::map<GLuint>(b, GL_WRITE_ONLY);
            auto idx = 0;
            for(auto& t : mesh.triangles) {
                mapping[idx++] = t[0];
                mapping[idx++] = t[1];
                mapping[idx++] = t[2];
            }
            agl::unmap(b);
            g.indexes = std::move(b);
        }
        { // Normals.
            auto b = agl::buffer();
            agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
            auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
            auto idx = 0;
            for(auto& v : mesh.vertices) {
                mapping[idx++] = v.normal;
            }
            agl::unmap(b);
            g.attributes["normal3"] = AttributeBuffer{
                .buffer = std::move(b),

                .size = agl::Size<GLint>(3),
                .stride = agl::Stride<GLsizei>(12),
                .type = GL_FLOAT,
            };
        }
        { // Positions.
            auto b = agl::buffer();
            agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
            auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
            auto idx = 0;
            for(auto& v : mesh.vertices) {
                mapping[idx++] = v.position;
            }
            agl::unmap(b);
            g.attributes["position3"] = AttributeBuffer{
                .buffer = std::move(b),

                .size = agl::Size<GLint>(3),
                .stride = agl::Stride<GLsizei>(12),
                .type = GL_FLOAT,
            };
        }
        { // Texcoords.
            auto b = agl::buffer();
            agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
            auto mapping = agl::map<agl::Vec2>(b, GL_WRITE_ONLY);
            auto idx = 0;
            for(auto& v : mesh.vertices) {
                mapping[idx++] = v.texcoords;
            }
            agl::unmap(b);
            g.attributes["texcoords2"] = AttributeBuffer{
                .buffer = std::move(b),

                .size = agl::Size<GLint>(2),
                .stride = agl::Stride<GLsizei>(8),
                .type = GL_FLOAT,
            };
        }
        rock_geometry = std::make_shared<Geometry>(std::move(g));
    }
    

    { // Subdivided cube.
        auto object = Object();
        object.geometry = rock_geometry;
        object.material = phong_material;
        object.uniforms["albedo_texture"] = 0;
        objects.push_back(std::move(object));
    }
    auto& rock = objects.back();

    auto baking_material = std::shared_ptr<Material>();
    {
        auto material = Material();
        { // Program.
            auto& p = material.program;
            auto shaders = std::map<agl::ShaderType, std::string>{
                {
                    agl::ShaderType::vertex,
                    file(root + "src/shader/rock/bake_texture.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(root + "src/shader/rock/bake_texture.fs")
                },
            };
            for(auto& [type, src] : shaders) {
                auto s = agl::shader(type);
                agl::source(s, src);
                agl::compile(s);
                agl::attach(p, s);
                agl::delete_(s);
            }
            agl::link(p);
        }
        baking_material = std::make_shared<Material>(std::move(material));
    }

    auto rock_texture = agl::Texture();
    // while(!glfwWindowShouldClose(window)) {
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
                render(o);
            }
        }
        // glfwSwapBuffers(window);
        // glfwPollEvents();
    }

    for(auto& o : objects) {
        configure(o);
    }

    {
        agl::use(phong_material->program);
        agl::uniform(phong_material->program, agl::uniform_location(phong_material->program, "albedo_texture"), 0);
        agl::bind_unit(0, rock_texture);
    }

    auto view = View();
    view.pitch = 0.5f;
    view.rotation_center[2] = -5;
    auto proj = PerspectiveProjection();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while(!glfwWindowShouldClose(window)) {
        agl::bind_default(agl::FramebufferTarget::framebuffer);
        glViewport(0, 0, 1280, 720);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            agl::Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw -= d[0] / 500.f;
            view.pitch -= d[1] / 500.f;

            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window, GLFW_KEY_S)) {
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l - 0.1f) / l;
            }
            if(glfwGetKey(window, GLFW_KEY_W)) {
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l + 0.1f) / l;
            }
        }

        for(auto& object : objects) {
            { // Uniforms.
                auto model = agl::mat4(agl::identity);
                {
                    auto it = object.uniforms.find("model");
                    if(it != end(object.uniforms)) {
                        model = std::any_cast<const agl::Mat4&>(it->second);
                    }
                }

                auto view_transform =  inverse(transform(view));

                auto mvp = transform(proj) * view_transform * model;
                object.uniforms["mvp"] = std::move(mvp);

                auto normal_transform = agl::transpose(inverse(view_transform * model));
                object.uniforms["normal_transform"] = std::move(normal_transform);
            }
            render(object);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

int main() {
    try {
        return throwing_main();
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}
