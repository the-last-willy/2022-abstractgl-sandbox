#include "agl/all.hpp"

#include "engine/all.hpp"
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
#include <typeindex>
#include <vector>

using namespace tlw;

template<typename... Types>
struct TypeList {};

using rtti_uniform_map = std::map<
    std::type_index,
    std::function<void(
        agl::Program, agl::UniformIndex<GLint>, std::any)>>;

template<typename Head, typename... Tail>
rtti_uniform_map make_rtti_uniforms(TypeList<Head, Tail...>) {
    auto t = make_rtti_uniforms(TypeList<Tail...>());
    t[std::type_index(typeid(Head))] = [](agl::Program p, agl::UniformIndex<GLint> ui, std::any a) {
        agl::uniform(p, ui, std::any_cast<Head>(a));
    };
    return t;
}

inline
rtti_uniform_map make_rtti_uniforms(TypeList<>) {
    return {};
}

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
    }

    auto objects = std::deque<Object>();

    auto tree_structure = turtle::Tree();
    {
        auto t = turtle::Turtle();
        tree_structure = t.feed2(file(root + "data/lsystem/1_25_7.txt"));
    }

    const auto rtti_to_uniform = make_rtti_uniforms(TypeList<
        agl::Mat4,
        agl::Vec3,
        agl::Vec4>());

    auto phong_material = std::make_shared<Material>(
        make_branch_material());

    auto cylinder_geometry = std::make_shared<Geometry>(
        make_cylinder_geometry());

    { // Cylinder joints.
        auto traverser = turtle::TreeTransforms();
        traverse(tree_structure, traverser);
        for(auto& t : traverser.transforms) {
            auto o = Object();
            o.geometry = cylinder_geometry;
            o.material = phong_material;

            o.uniforms["model"] = t
            * agl::rotation_z(agl::pi / 2.f)
            * agl::scaling3(0.1f, 0.5f, 0.1f)
            * agl::translation(0.f, 1.f, 0.f);
            
            objects.push_back(std::move(o));
        }
    }

    auto leaf_material = std::make_shared<Material>(
        make_leaf_material());

    { // Leaves.
        auto traverser = turtle::TreeLeaves();
        traverse(tree_structure, traverser);

        auto leaf = std::make_shared<Geometry>();
        leaf->draw_mode = agl::DrawMode::triangle_fan;

        for(std::size_t i = 0; i < traverser.leaves.size(); ++i) {
            auto tp = traverser.leaves[i];
            auto object = Object();
            object.material = leaf_material;

            auto& geometry = *(object.geometry = std::make_shared<Geometry>(*leaf));

            geometry.primitive_count = agl::Count<GLsizei>(
                static_cast<GLsizei>(size(tp->boundary)));

            { // Positions.
                auto b = agl::buffer();
                agl::storage(b, std::span(tp->boundary));
                geometry.attributes["position3"] = AttributeBuffer{
                    .buffer = std::move(b),

                    .size = agl::Size<GLint>(sizeof(GLfloat)),
                    .stride = agl::Stride<GLsizei>(3),
                    .type = GL_FLOAT
                };
            }
            { // Normals.
                auto& b = tp->boundary;
                auto n = agl::normalize(agl::cross(b[1] - b[0], b[2] - b[0]));
                auto normals = std::vector<agl::Vec3>(size(b), n);
                auto buf = agl::buffer();
                agl::storage(buf, std::span(normals));
                geometry.attributes["normal3"] = AttributeBuffer{
                    .buffer = std::move(buf),

                    .size = agl::Size<GLint>(sizeof(GLfloat)),
                    .stride = agl::Stride<GLsizei>(3),
                    .type = GL_FLOAT
                };
            }

            objects.push_back(std::move(object));
        }
    }

    for(auto& o : objects) {
        auto geometry = *o.geometry;
        auto program = o.material->program;

        auto va = o.vertex_array;

        if(geometry.indexes) {
            element_buffer(va, *geometry.indexes);
        }
        for(int i = 0; i < agl::active_attributes(program); ++i) {
            auto ai = agl::AttributeIndex(i);
            auto aa = agl::active_attrib(program, ai);
            auto attribute = geometry.attributes.at(aa.name);

            auto bi = agl::BindingIndex<GLuint>(i);

            attribute_binding(va, ai, bi);
            attribute_format(va, ai,
                attribute.size, attribute.type);
            vertex_buffer(va, bi,
               attribute.buffer, attribute.offset, attribute.stride);
            enable(va, ai);
        }
    }

    auto view = View();
    view.pitch = 0.5f;
    view.rotation_center[2] = -5;
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
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l - 0.1f) / l;
            }
            if(glfwGetKey(window, GLFW_KEY_W)) {
                auto l = agl::length(view.rotation_center);
                view.rotation_center *= (l + 0.1f) / l;
            }
        }

        for(auto& object : objects) {
            auto& geometry = *object.geometry;
            auto& material = *object.material;

            for(auto& p : material.parameters) {
                p();
            }
            for(auto c : material.capabilities) {
                agl::enable(c);
            }

            auto program = material.program;
            agl::use(program);

            { // Uniforms
                auto model = agl::mat4(agl::identity);
                {
                    auto it = object.uniforms.find("model");
                    if(it != end(object.uniforms)) {
                        model = std::any_cast<const agl::Mat4&>(it->second);
                    }
                }

                auto view_transform =  inverse(transform(view)) * agl::scaling3(1 / 5.f);

                auto mvp = transform(proj) * view_transform * model;
                object.uniforms["mvp"] = std::move(mvp);

                auto normal_transform = agl::transpose(inverse(view_transform * model));
                object.uniforms["normal_transform"] = std::move(normal_transform);

                for(int idx = 0; idx < agl::active_uniforms(program); ++idx) {
                    auto ui = agl::UniformIndex(idx);
                    auto info = agl::active_uniform(
                        program, agl::UniformIndex<GLuint>(ui));
                    auto it = object.uniforms.find(info.name);
                    if(it != end(object.uniforms)) {
                        auto type_idx = std::type_index(it->second.type());
                        rtti_to_uniform.at(type_idx)(program, ui, it->second);
                    }
                }
            }
            {
                agl::bind(object.vertex_array);

                if(geometry.indexes) {
                    agl::draw_elements(
                        geometry.draw_mode,
                        geometry.primitive_count,
                        agl::DrawType::unsigned_int);
                } else {
                    agl::draw_arrays(
                        geometry.draw_mode,
                        agl::Offset<GLint>(0),
                        geometry.primitive_count);
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
