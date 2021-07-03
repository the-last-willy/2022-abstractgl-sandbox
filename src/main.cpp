#include "gl/all.hpp"
#include "scene/all.hpp"

#include "matrix/all.hpp"
#include "vector/all.hpp"

#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <fstream>
#include <iostream>
#include <span>
#include <vector>
 
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
    
        window = glfwCreateWindow(1280, 720, "Spot on.", NULL, NULL);
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

    auto position_indices = std::vector<unsigned>{};
    auto positions = std::vector<Vec3>{};
    {
        std::string inputfile = root + "/data/wavefront/spot/spot_triangulated.obj";
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
        if (!err.empty()) { // `err` may contain warning message.
            std::cerr << err << std::endl;
        }

        if (!ret) {
            exit(1);
        }

        auto& shape = shapes.front();

        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                position_indices.push_back(static_cast<unsigned>(idx.vertex_index));
                positions.push_back(vec3(vx, vy, vz));

                // tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                // tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                // tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                // tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                // tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

            }
            index_offset += fv;
        }
    }

    auto normals = std::vector<Vec3>{};
    {
        // for(auto v : position_indices) {
        //     for(i)
        // }
    }

    // const auto vertex_colors = std::array{
    //     Vec3{0.f, 0.f, 0.f},
    //     Vec3{0.f, 0.f, 1.f},
    //     Vec3{0.f, 1.f, 0.f},
    //     Vec3{0.f, 1.f, 1.f},
    //     Vec3{1.f, 0.f, 0.f},
    //     Vec3{1.f, 0.f, 1.f},
    //     Vec3{1.f, 1.f, 0.f},
    //     Vec3{1.f, 1.f, 1.f}
    // };

    // auto vertex_positions = std::array{
    //     Vec3{-0.5f, -0.5f, -0.5f},
    //     Vec3{-0.5f, -0.5f,  0.5f},
    //     Vec3{-0.5f,  0.5f, -0.5f},
    //     Vec3{-0.5f,  0.5f,  0.5f},
    //     Vec3{ 0.5f, -0.5f, -0.5f},
    //     Vec3{ 0.5f, -0.5f,  0.5f},
    //     Vec3{ 0.5f,  0.5f, -0.5f},
    //     Vec3{ 0.5f,  0.5f,  0.5f}
    // };

    // const auto triangle_indices = std::array{
    //     0u, 1u, 2u, 1u, 3u, 2u, // -x face
    //     4u, 6u, 5u, 7u, 5u, 6u, // +x face
    //     0u, 4u, 1u, 4u, 5u, 1u, // -y face
    //     2u, 3u, 6u, 7u, 6u, 3u, // +y face
    //     0u, 2u, 4u, 6u, 4u, 2u, // -z face
    //     1u, 5u, 3u, 7u, 3u, 5u  // +z face
    // };

    auto va = gl::VertexArray();
    gl::bind(va);

    auto positions_buffer = gl::Buffer();
    {
        gl::bind_to_array(positions_buffer);
        gl::storage(positions_buffer, std::span(positions));

        gl::enable(va, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    }

    // auto element_array_buffer = gl::Buffer();
    // {
    //     gl::bind_to_element_array(element_array_buffer);
    //     gl::storage(element_array_buffer, std::span(indices));
    // }

    auto program = gl::Program();
    {
        auto vs = vertex_shader();
        {
            source(vs, file(root + "data/shader/flat_shading.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(program, vs);
        }

        auto gs = geometry_shader();
        {
            source(gs, file(root + "data/shader/flat_shading.gs"));
            auto success = gl::try_compile(gs);
            if(success) {
                std::cout << "-- Geometry shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(gs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(program, gs);
        }

        auto fs = fragment_shader();
        {
            source(fs, file(root + "data/shader/flat_shading.fs"));
            auto success = gl::try_compile(fs);
            if(success) {
                std::cout << "-- Fragment shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(fs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(program, fs);
        }

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

    auto camera = scene::Camera();
    y(camera.position) += 0.f;
    camera.pitch += pi / 8.f;
    camera.offset = 2.1f;

    auto object_position_1 = Vec3{0.f, 0.f, 0.f};

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_A)) {
            camera.yaw += 0.02f;
        }
        // if(glfwGetKey(window, GLFW_KEY_D))
        {
            camera.yaw -= 0.002f;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN)) {
            camera.pitch += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            camera.position = camera.position
            + xyz(scene::rotation(camera) * Vec4{0.f, 0.f, -0.02f, 1.f});
        }
        if(glfwGetKey(window, GLFW_KEY_UP)) {
            camera.pitch -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            camera.position = camera.position
            + xyz(scene::rotation(camera) * Vec4{0.f, 0.f, 0.02f, 1.f});
        }

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::use(program);

        auto view = scene::transformation(camera);
        auto projection = scene::perspective(camera);

        {
            auto model = translation(object_position_1);
            auto mv = view * model;
            auto mvp = projection * view * model;

            glUniformMatrix4fv(
                gl::uniform_location(program, "m"),
                1, GL_FALSE, model.elements.data());
            glUniformMatrix4fv(
                gl::uniform_location(program, "mvp"),
                1, GL_FALSE, mvp.elements.data());
            gl::draw_arrays(GL_TRIANGLES, 0, positions.size());
        }
 
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
