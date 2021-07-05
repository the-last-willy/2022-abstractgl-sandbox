#include "gl/all.hpp"
#include "scene/all.hpp"
#include "vector/all.hpp"
#include "file.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

using namespace tlw;

struct Vertex {
    Vec3 normal = {};
    Vec3 position = {};
    Vec2 uv = {};
};

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
    GLFWwindow* window;
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    
        window = glfwCreateWindow(1280, 720, "Audi.", NULL, NULL);
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

    // Loading the model.
    auto buffers = std::vector<gl::Buffer>();
    auto vertex_counts = std::vector<GLsizei>();
    auto vertex_arrays = std::vector<gl::VertexArray>();
    auto materials_ = std::vector<tinyobj::material_t>();
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        {   
            auto err = std::string{};
            auto base_dir = std::string("D:/data/3d_model/2020_audi_rs_6_avant");
            auto filepath = base_dir + "/Audi_RS_6_Avant.obj";
            auto warn = std::string{};
            bool ret = tinyobj::LoadObj(
                &attrib,
                &shapes,
                &materials,
                &warn,
                &err,
                filepath.c_str(),
                base_dir.c_str());

            if (!warn.empty()) {
                std::cout << warn << std::endl;
            }

            if (!err.empty()) {
                std::cerr << err << std::endl;
            }

            if (!ret) {
                exit(1);
            }
        }

        buffers.resize(shapes.size());
        materials_.resize(shapes.size());
        vertex_counts.resize(shapes.size());
        vertex_arrays.resize(shapes.size());

        for(std::size_t i = 0; i < shapes.size(); ++i) {
            const auto& s = shapes[i];
            auto& m = s.mesh;

            auto& buffer = buffers[i];
            auto& vertex_array = vertex_arrays[i];
            
            vertex_counts[i] = static_cast<GLsizei>(m.indices.size());

            materials_[i] = materials[m.material_ids.front()];

            auto vertices = std::vector<Vertex>();
            for(auto& idx : m.indices) {
                auto normal = Vec3{
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]};
                auto position = Vec3{
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]};
                auto uv = Vec2{
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]};
                vertices.push_back(Vertex{
                    .normal = normal,
                    .position = position,
                    .uv = uv});
            }

            gl::bind_to_array(buffer);
            gl::storage(buffer, std::span(vertices));

            gl::bind(vertex_array);

            gl::enable(vertex_array, 0);
            glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0 * sizeof(GL_FLOAT)));

            gl::enable(vertex_array, 1);
            glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GL_FLOAT)));

            gl::enable(vertex_array, 2);
            glVertexAttribPointer(
                2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GL_FLOAT)));
        }
    }

    // Loading the shader.
    auto program = gl::Program();
    {
        auto vs = gl::VertexShader();
        {
            gl::source(vs, file(root + "src/shader/phong.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(program, vs);
        }

        auto fs = gl::FragmentShader();
        {
            gl::source(fs, file(root + "src/shader/phong.fs"));
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
    }

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    auto camera = scene::Camera();
    camera.offset = 3.f;
    camera.pitch = 0.5f;
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.yaw += 0.01f;

        auto normal_matrix = mat4_identity();
        auto v = transformation(camera);
        auto vp = perspective(camera) * v;

        auto light_dir = xyz(v * normalized(Vec4{-1.f, -1.f, 0.f, 0.f}));

        for(std::size_t i = 0; i < vertex_arrays.size(); ++i) {
            gl::use(program);
            {
                auto& mat = materials_[i];
                gl::try_uniform(program, "light_dir",
                    light_dir);
                gl::try_uniform(program, "material.ambient",
                    Vec3{mat.ambient[0], mat.ambient[1], mat.ambient[2]});
                gl::try_uniform(program, "material.diffuse",
                    Vec3{mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]});
                gl::try_uniform(program, "material.shininess",
                    mat.shininess);
                gl::try_uniform(program, "material.specular",
                    Vec3{mat.specular[0], mat.specular[1], mat.specular[2]});
            }
            gl::try_uniform(program, "mv", v);
            gl::try_uniform(program, "mvp", vp);
            gl::try_uniform(program, "normal_matrix", normal_matrix);
            gl::bind(vertex_arrays[i]);
            gl::draw_arrays(GL_TRIANGLES, 0, vertex_counts[i]);
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
