#include "gl/all.hpp"

#include "matrix/all.hpp"
#include "vector/all.hpp"

#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <fstream>
#include <iostream>
 
using namespace tlw;

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

int throwing_main(void) {
    GLFWwindow* window;
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    
        window = glfwCreateWindow(1280, 720, "Cube.", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    
        glfwSetKeyCallback(window, key_callback);
    
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            printf("Failed to initialize OpenGL context");
            return -1;
        }
    }

    const auto vertex_colors = std::array{
        Vec3{0.f, 0.f, 0.f},
        Vec3{0.f, 0.f, 1.f},
        Vec3{0.f, 1.f, 0.f},
        Vec3{0.f, 1.f, 1.f},
        Vec3{1.f, 0.f, 0.f},
        Vec3{1.f, 0.f, 1.f},
        Vec3{1.f, 1.f, 0.f},
        Vec3{1.f, 1.f, 1.f}
    };

    auto vertex_positions = std::array{
        Vec3{-0.5f, -0.5f, -0.5f},
        Vec3{-0.5f, -0.5f,  0.5f},
        Vec3{-0.5f,  0.5f, -0.5f},
        Vec3{-0.5f,  0.5f,  0.5f},
        Vec3{ 0.5f, -0.5f, -0.5f},
        Vec3{ 0.5f, -0.5f,  0.5f},
        Vec3{ 0.5f,  0.5f, -0.5f},
        Vec3{ 0.5f,  0.5f,  0.5f}
    };

    const auto triangle_indices = std::array{
        0u, 3u, 1u, 0u, 2u, 3u, // -x face
        4u, 7u, 5u, 4u, 6u, 7u, // +x face
        0u, 1u, 5u, 0u, 5u, 4u, // -y face
        2u, 7u, 3u, 2u, 6u, 7u, // +y face
        0u, 6u, 2u, 0u, 4u, 6u, // -z face
        1u, 3u, 7u, 1u, 7u, 5u  // +z face
    };

    auto va = gl::VertexArray();
    gl::bind(va);

    auto colors_buffer = gl::Buffer();
    {
        gl::bind_to_array(colors_buffer);
        gl::storage(colors_buffer, std::span(vertex_colors));

        gl::enable(va, 1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, 0);
    }

    auto positions_buffer = gl::Buffer();
    {
        gl::bind_to_array(positions_buffer);
        gl::storage(positions_buffer, std::span(vertex_positions));

        gl::enable(va, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    }

    auto element_array_buffer = gl::Buffer();
    {
        gl::bind_to_element_array(element_array_buffer);
        gl::storage(element_array_buffer, std::span(triangle_indices));
    }

    auto program = gl::Program();
    {
        auto vs = vertex_shader();
        {
            source(vs, file(root + "data/shader/pass_through.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
        }

        auto fs = fragment_shader();
        {
            source(fs, file(root + "data/shader/pass_through.fs"));
            auto success = gl::try_compile(fs);
            if(success) {
                std::cout << "-- Fragment shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(fs) << std::endl;
                throw gl::CompilationFailure();
            }
        }

        gl::attach(program, vs);
        gl::attach(program, fs);
        {
            auto success = gl::try_link(program);
            if(success) {
                std::cout << "-- Program linkage succeeded." << std::endl;
            } else {
                std::cerr << gl::info_log(program) << std::endl;
                throw gl::LinkageFailure();
            }
        }
        gl::use(program);
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    auto camera_position = Vec3{0.f, 0.f, 0.f};
    auto camera_y_angle = 0.f;

    auto object_x_angle = 0.f;
    auto object_y_angle = 0.f;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_A)) {
            camera_y_angle -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_D)) {
            camera_y_angle += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN)) {
            object_x_angle -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT)) {
            object_y_angle -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
            object_y_angle += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            camera_position = camera_position + Vec3{0.f, 0.f, -0.01f};
        }
        if(glfwGetKey(window, GLFW_KEY_UP)) {
            object_x_angle += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            camera_position = camera_position + Vec3{0.f, 0.f, 0.01f};
        }

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::use(program);

        {
            auto model = rotation_y(object_y_angle) * rotation_x(object_x_angle) * scaling(0.5f) * translation(0.f, 0.f, 0.5f);
            auto view = translation(camera_position);
            auto mvp = view * model;
            glUniformMatrix4fv(
                gl::uniform_location(program, "mvp"),
                1, GL_FALSE, mvp.elements.data());
        }
        
        gl::draw_elements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*) 0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

int main() {
    try {
        return throwing_main();
    } catch(const gl::Error& e) {
        std::cerr << "tlw::gl::Error(" << std::hex << e.category() << ")" << std::endl;
    } catch(const gl::CompilationFailure&) {
        std::cerr << "tlw::gl::CompilationFailure" << std::endl;
    } catch(const gl::LinkageFailure&) {
        std::cerr << "tlw::gl::LinkageFailure" << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}
