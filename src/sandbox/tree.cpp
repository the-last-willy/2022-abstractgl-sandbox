#include "agl/all.hpp"

#include "scene/all.hpp"
#include "turtle/all.hpp"
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
#include <vector>

using namespace tlw;

constexpr
auto perpendicular(agl::Vec3 v) {
    return agl::vec3(
        v[1] - v[2],
        v[2] - v[0],
        v[0] - v[1]);
}

inline
agl::Mat4 lookat(agl::Vec3 dir) {
    auto p = agl::normalize(perpendicular(dir));
    return agl::mat4(
        agl::vec4(p, 0.f),
        agl::vec4(dir, 0.f),
        agl::vec4(agl::cross(p, dir), 0.f),
        agl::vec4(0.f, 0.f, 0.f, 1.f));
}

struct AttributeManager {
    // Attribute2& at(const std::string& name) {
    //     return name_to_attribute.at(name);
    // }

    // void insert(Attribute2&& a) {
    //     attributes.push_back(std::move(a));
    //     name_to_attribute[std::cref(a.name)] = std::ref(attributes.back());
    // }

    // std::vector<Attribute2> attributes = {};

    // std::map<
    //     std::reference_wrapper<const std::string>,
    //     std::reference_wrapper<Attribute2>>
    //         name_to_attribute = {};
};

struct Attribute {
    agl::Size<GLint> size = agl::Size<GLint>(0);
    agl::Stride<GLsizei> stride = agl::Stride<GLsizei>(0);
    GLenum type;
};

struct Transform {
    agl::Vec3 translation = agl::vec3(agl::null);
    agl::Mat4 rotation = agl::mat4(agl::identity);
};

inline
agl::Mat4 mat(const Transform& t) {
    return agl::translation(t.translation)
    * t.rotation;
}

struct Material {
    std::vector<agl::Capability> capabilities = {};
    agl::Program program = agl::program();
    std::vector<std::function<void()>> parameters = {};
};

struct Object {
    std::map<std::string, agl::Buffer> attributes = {};
    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    std::optional<agl::Buffer> index_buffer = std::nullopt;
    Material material = {};
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
    std::map<std::string, std::any> uniforms = {};
    agl::VertexArray vertex_array = agl::vertex_array();
};

// inline
// Mat4 world_transformation(const Object& o) {
//     return agl::translation(o.transform.translation)
// }

Vec2 previous_cursor_pos;
Vec2 current_cursor_pos;

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
        glDebugMessageCallback(&gl::error_callback, NULL);
        gl::throw_if_error();
    }

    auto skeleton = turtle::Joint();
    {
        auto t = turtle::Turtle();
        skeleton = t.feed(R"(A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//-F+|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//F|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//+F-|CFB-F+B|FA&F^A&&FB-F+B|FC//&F^|CFB-F+B|FA&F^A&&FB-F+B|FC//-F+&&|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//F|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//+F+A&F^CFB^F^D^^-F-D^|F^B|FC^F^A////)");
        // skeleton = t.feed(R"(B-F+CFC+F-D&F^D-F+&&CFC+F+B//)");
        print(skeleton);
    }

    auto attributes = std::map<std::string, Attribute>{};
    attributes["normal3"] = Attribute{
        .size = agl::Size<GLint>(3),
        .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
        .type = GL_FLOAT
    };
    attributes["position3"] = Attribute{
        .size = agl::Size<GLint>(3),
        .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
        .type = GL_FLOAT
    };
    attributes["uv"] = Attribute{
        .size = agl::Size<GLint>(2),
        .stride = agl::Stride<GLsizei>(2 * sizeof(GLfloat)),
        .type = GL_FLOAT
    };

    auto line_material = Material();
    {
        { // Capabilities.
            line_material.capabilities.insert(
                end(line_material.capabilities),
                {
                    agl::Capability::depth_test,
                    agl::Capability::line_smooth,
                });
        }
        { // Parameters.
            line_material.parameters.push_back([&line_material]() {
                glDepthFunc(GL_LESS);
                agl::uniform(
                    line_material.program,
                    agl::uniform_location(line_material.program, "color_rgb"),
                    agl::vec3(1.f, 1.f, 1.f));
            });
        }
        { // Program.
            auto& p = line_material.program;
            auto shaders = std::map<agl::ShaderType, std::string>{
                {
                    agl::ShaderType::vertex,
                    file(root + "src/shader/tree/line.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(root + "src/shader/tree/line.fs")
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
    }

    auto phong_material = Material();
    {
        { // Capabilities.
            phong_material.capabilities.insert(
                end(phong_material.capabilities),
                {
                    agl::Capability::depth_test,
                    agl::Capability::line_smooth,
                });
        }
        { // Parameters.
            phong_material.parameters.push_back([]() {
                glCullFace(GL_BACK);
                glDepthFunc(GL_LESS);
            });
        }
        { // Program.
            auto& p = phong_material.program;
            auto shaders = std::map<agl::ShaderType, std::string>{
                {
                    agl::ShaderType::vertex,
                    file(root + "src/shader/tree/phong.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(root + "src/shader/tree/phong.fs")
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
    }

    auto objects = std::deque<Object>();

    auto cylinder = Object();
    {
        constexpr auto resolution = 10;
        cylinder.primitive_count = agl::Count<GLsizei>(3 * 2 * resolution);
        { // Normals.
            auto data = std::array<agl::Vec3, 2 * resolution>{};
            for(int i = 0; i < resolution; ++i) {
                auto c = std::cos(tau * i / resolution);
                auto s = std::sin(tau * i / resolution);
                data[i             ] = agl::vec3(c, 0.f, s);
                data[i + resolution] = agl::vec3(c, 0.f, s);
            }
            auto b = agl::buffer();
            agl::storage(b, std::span(data));
            cylinder.attributes["normal3"] = std::move(b);
        }
        { // Positions
            auto data = std::array<agl::Vec3, 2 * resolution>{};
            for(int i = 0; i < resolution; ++i) {
                auto c = std::cos(tau * i / resolution);
                auto s = std::sin(tau * i / resolution);
                data[i             ] = agl::vec3(c, -1.f, s);
                data[i + resolution] = agl::vec3(c,  1.f, s);
            }
            auto b = agl::buffer();
            agl::storage(b, std::span(data));
            cylinder.attributes["position3"] = std::move(b);
        }
        {
            auto data = std::array<GLuint, 3 * 2 * resolution>{};
            for(int i = 0; i < resolution; ++i) {
                // Lower triangle.
                data[2 * 3 * i + 0 + 0] = i;
                data[2 * 3 * i + 0 + 1] = (i + 1) % resolution;
                data[2 * 3 * i + 0 + 2] = i + resolution;
                // Upper triangle.
                data[2 * 3 * i + 3 + 0] = (i + 1) % resolution + resolution;
                data[2 * 3 * i + 3 + 1] = i + resolution;
                data[2 * 3 * i + 3 + 2] = (i + 1) % resolution;
            }
            auto b = agl::buffer();
            agl::storage(b, std::span(data));
            cylinder.index_buffer = std::move(b);
            cylinder.material = phong_material;
        }
    }

    { // Cylinder joints.
        auto depth_traversal = [&](turtle::Joint& j, agl::Vec3 offset, auto rec) -> void {
            auto o2 = j.offset + offset;
            {
                auto c = cylinder;
                c.uniforms["model"] = std::make_any<agl::Mat4>(
                    agl::translation(o2)
                    * lookat(agl::normalize(o2 - offset))
                    * agl::scaling3(0.1f, 1.f, 0.1f)
                    * agl::mat4(
                        agl::vec4(1.f,   0.f, 0.f, 0.f),
                        agl::vec4(0.f,  0.5f, 0.f, 0.f),
                        agl::vec4(0.f,   0.f, 1.f, 0.f),
                        agl::vec4(0.f, -0.5f, 0.f, 1.f)));
                objects.push_back(std::move(c));
            }
            for(auto& c : j.children) {
                rec(c, o2, rec);
            }
        };
        for(auto& c : skeleton.children) {
            depth_traversal(c, skeleton.offset, depth_traversal);
        }
    }

    { // Cylinder lines.
        auto lines = Object();
        lines.draw_mode = agl::DrawMode::lines;
        lines.material = line_material;
        auto positions = std::vector<agl::Vec3>();
        auto depth_traversal = [&](turtle::Joint& j, agl::Vec3 offset, auto rec) -> void {
            auto o2 = j.offset + offset;
            {
                positions.push_back(offset);
                positions.push_back(o2);
            }
            for(auto& c : j.children) {
                rec(c, o2, rec);
            }
        };
        for(auto& c : skeleton.children) {
            depth_traversal(c, skeleton.offset, depth_traversal);
        }
        lines.primitive_count = agl::Count<GLsizei>(
            static_cast<GLsizei>(size(positions)));
        {
            auto b = agl::buffer();
            storage(b, std::span(positions));
            lines.attributes["position3"] = std::move(b);
        }
        objects.push_back(std::move(lines));
    }

    auto quad = Object();
    {
        quad.primitive_count = agl::Count<GLsizei>(6);
        {
            auto data = std::array{
                agl::vec3(-1, 0, -1), agl::vec3(-1, 0, 1),
                agl::vec3( 1, 0, -1), agl::vec3( 1, 0, 1),};
            auto b = agl::buffer();  
            agl::storage(b, std::span<agl::Vec3>(data), 0);
            quad.attributes["position3"] = std::move(b);
        }
        {
            auto data = std::array{
                0u, 1u, 2u,
                1u, 3u, 2u};
            auto b = agl::buffer();
            agl::storage(b, std::span<GLuint>(data), 0);
            quad.index_buffer = std::move(b);
        }
    }

    for(const auto& o : objects) {
        auto p = o.material.program;
        auto& va = o.vertex_array;

        if(o.index_buffer) {
            element_buffer(va, *o.index_buffer);
        }
        for(int i = 0; i < agl::active_attributes(p); ++i) {
            auto ai = agl::AttributeIndex(i);
            auto aa = agl::active_attrib(p, ai);
            auto a = attributes.at(aa.name);

            auto bi = agl::BindingIndex<GLuint>(i);

            attribute_binding(va, ai, bi);
            attribute_format(va, ai, a.size, a.type);
            vertex_buffer(va, bi,
                o.attributes.at(aa.name), agl::Offset<GLintptr>(0), a.stride);
            enable(va, ai);
        }
    }

    auto view = View();
    view.pitch = 0.5f;
    view.rotation_center[2] = -20;
    auto proj = PerspectiveProjection();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while(!glfwWindowShouldClose(window)) {
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw -= d.at(0) / 500.f;
            view.pitch -= d.at(1) / 500.f;

            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window, GLFW_KEY_S)) {
                view.position = view.position
                + (transform(view) * agl::vec4(0.f, 0.f, -1.f, 0.f)).xyz();
            }
            if(glfwGetKey(window, GLFW_KEY_W)) {
                view.position = view.position
                + (transform(view) * agl::vec4(0.f, 0.f, 1.f, 0.f)).xyz();
            }
        }

        for(int i = 0; i < size(objects); ++i) {
            auto& o = objects[i];
            auto& material = o.material;

            for(auto& p : material.parameters) {
                p();
            }
            for(auto c : material.capabilities) {
                agl::enable(c);
            }

            auto program = o.material.program;
            agl::use(program);

            {
                auto model = agl::mat4(agl::identity);
                {
                    auto it = o.uniforms.find("model");
                    if(it != end(o.uniforms)) {
                        // auto& t = std::any_cast<const Transform&>(it->second);
                        // model = mat(t);
                        auto& t = std::any_cast<const agl::Mat4&>(it->second);
                        model = t;
                    }
                }

                auto mvp = transform(proj) * inverse(transform(view)) * model;
                agl::uniform(program,
                    agl::uniform_location(program, "mvp"), mvp);
            }
            {
                auto va = o.vertex_array;
                agl::bind(va);

                if(o.index_buffer) {
                    agl::draw_elements(
                        o.draw_mode,
                        o.primitive_count,
                        agl::DrawType::unsigned_int);
                } else {
                    agl::draw_arrays(
                        o.draw_mode,
                        agl::Offset<GLint>(0),
                        o.primitive_count);
                }
            }

            for(auto c : material.capabilities) {
                agl::disable(c);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

int main() {
    try {
        return throwing_main();
    } catch(const gl::Error& e) {
        std::cerr << "tlw::gl::Error(" << std::hex << e.category() << ")" << std::endl;
    } catch(const gl::CompilationFailure&) {
        std::cerr << "tlw::gl::CompilationFailure" << std::endl;
    } catch(const gl::IncompleteFrameBuffer& i) {
        std::cerr << "tlw::gl::IncompleteFrameBuffer: " << std::hex << i.category() << std::endl;
    } catch(const gl::InvariantViolation&) {
        std::cerr << "tlw::gl::InvariantViolation" << std::endl;
    } catch(const gl::LinkageFailure&) {
        std::cerr << "tlw::gl::LinkageFailure" << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}
