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
    
    auto scene = scene::Group<scene::Renderer>();
    auto car = std::make_shared<scene::Group<scene::Renderer>>();

    for(int i = 0; i < 10; ++i) {
        auto t = scaling(0.2) * translation(Vec3{float(1.6f * i) - 6.f, 0.f, 0.f});
        scene.children.emplace_back(new scene::Transformation<scene::Renderer>(t, car));
    }

    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        {   
            auto err = std::string{};
            auto base_dir = std::string("D:/data/3d_model/the_cabin");
            auto filepath = base_dir + "/Lighting_Challenge_24_theCabin.obj";
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

        for(std::size_t i = 0; i < shapes.size(); ++i) {
            const auto& s = shapes[i];
            auto& m = s.mesh;

            auto instance = new scene::Instance<scene::Renderer>();
            car->children.emplace_back(instance);

            instance->material = materials[m.material_ids.front()];
            instance->vertex_count = static_cast<GLsizei>(m.indices.size());
            
            auto& buffer = instance->buffer;
            auto& vertex_array = instance->vertex_array;

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

    glClearColor(0.f, 1.f, 0.f, 0.f);

    scene::Camera camera = {};
    camera.offset = 3.f;
    camera.pitch = 0.5f;

    auto renderer = scene::Renderer();
    renderer.program = &program;
    
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto p = perspective(camera);
        renderer.projection = p;

        auto v = transformation(camera);
        renderer.view = v;

        camera.yaw += 0.002f;
        auto camera_pos = xyz(rotation(camera) * vec4(0.f, 0.f, -camera.offset, 1.f));

        auto light_dir = normalized(Vec3{0.f, -1.f, 0.f});

        gl::use(program);

        gl::try_uniform(program, "camera_pos",
            camera_pos);
        gl::try_uniform(program, "light_dir",
            light_dir);

        car->accept(renderer);
        
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
