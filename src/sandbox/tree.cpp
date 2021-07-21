#include "agl/all.hpp"

#include "scene/all.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <span>
#include <vector>

using namespace tlw;

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

// inline
// agl::Mat4 mat(const Transform& t) {
//     return agl::translation(t.translation)
//         * t.rotation;
// }

struct Object {
    std::map<std::string, agl::Buffer> attributes = {};
    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
    Transform transform = {};
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

    auto attributes = std::map<std::string, Attribute>{};
    attributes["position3"] = Attribute{
        .size = agl::Size<GLint>(3),
        .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
        .type = GL_FLOAT
    };

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

    auto cylinder = Object();
    {
        // auto resolution = 10;
        // quad.primitive_count = agl::Count<GLsizei>(6);
        // {

        // }
        // {

        // }
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
            quad.attributes["index"] = std::move(b);
        }
    }

    auto phong = agl::program();
    {
        auto shaders = std::map<agl::ShaderType, std::string>{
            {
                agl::ShaderType::vertex,
                file(root + "src/shader/pass_through.vs")
            },
            {
                agl::ShaderType::fragment,
                file(root + "src/shader/pass_through.fs")
            },
        };
        for(auto& [type, src] : shaders) {
            auto s = agl::shader(type);
            agl::source(s, src);
            agl::compile(s);
            agl::attach(phong, s);
            agl::delete_(s);
        }
        agl::link(phong);
    }

    auto quad_phong = agl::vertex_array();
    {
        element_buffer(quad_phong, quad.attributes.at("index"));
        for(int i = 0; i < agl::active_attributes(phong); ++i) {
            auto ai = agl::AttributeIndex(i);
            auto aa = agl::active_attrib(phong, ai);
            auto a = attributes.at(aa.name);

            auto bi = agl::BindingIndex(0u);

            attribute_binding(quad_phong, ai, bi);
            attribute_format(quad_phong, ai, a.size, a.type);
            vertex_buffer(quad_phong, bi,
                quad.attributes.at(aa.name), agl::Offset<GLintptr>(0), a.stride);
            enable(quad_phong, ai);
        }
    }

    auto view = View();
    view.pitch = 0.5f;
    view.rotation_center[2] = -5;
    auto proj = PerspectiveProjection();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

        auto mvp = transform(proj) * inverse(transform(view));
        auto m = inverse(transform(view));
        if(glfwGetKey(window, GLFW_KEY_SPACE)) {
            for(int r = 0; r < 4; ++r) {
                for(int c = 0; c < 4; ++c) {
                    std::cout << m[c][r] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        {
            agl::use(phong);
            agl::uniform(phong,
                agl::uniform_location(phong, "mvp"), mvp);
            agl::bind(quad_phong);

            agl::draw_elements(
                agl::DrawMode::triangles,
                quad.primitive_count,
                agl::DrawType::unsigned_int);
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
