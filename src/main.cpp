#include "gl/all.hpp"
#include "scene/all.hpp"

#include "matrix/all.hpp"
#include "vector/all.hpp"

#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <fstream>
#include <iostream>
#include <span>
#include <vector>
 
//

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

    auto normals = std::vector<Vec3>{};
    auto positions = std::vector<Vec3>{};
    auto uvs = std::vector<Vec2>{};
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
                positions.push_back(vec3(vx, vy, vz));

                // tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                // tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                // tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];

                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                uvs.push_back(Vec2{tx, ty});
            }
            index_offset += fv;
        }
    }

    { // Normals computation.
        normals.resize(positions.size());
        for(std::size_t n = 0; n < size(normals); ++ n) {
            auto normal = vec3(0.f, 0.f, 0.f);
            auto pn = positions[n];
            for(std::size_t p = 0; p < size(positions); p += 3) {
                auto p0 = positions[p];
                auto p1 = positions[p + 1];
                auto p2 = positions[p + 2];
                if(p0 == pn || p1 == pn || p2 == pn) {
                    normal += cross(p1 - p0, p2 - p0);
                }
                
            }
            normals[n] = normalized(normal);
        }
    }

    auto va = gl::VertexArray();
    gl::bind(va);

    auto normals_buffer = gl::Buffer();
    {
        gl::bind_to_array(normals_buffer);
        gl::storage(normals_buffer, std::span(normals));

        gl::enable(va, 2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }

    auto positions_buffer = gl::Buffer();
    {
        gl::bind_to_array(positions_buffer);
        gl::storage(positions_buffer, std::span(positions));

        gl::enable(va, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }

    auto uvs_buffer = gl::Buffer();
    {
        gl::bind_to_array(uvs_buffer);
        gl::storage(uvs_buffer, std::span(uvs));

        gl::enable(va, 1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    }

    auto program = gl::Program();
    {
        auto vs = gl::VertexShader();
        {
            source(vs, file(root + "src/shader/texture.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(program, vs);
        }

        // auto gs = gl::GeometryShader();
        // {
        //     source(gs, file(root + "data/shader/flat_shading.gs"));
        //     auto success = gl::try_compile(gs);
        //     if(success) {
        //         std::cout << "-- Geometry shader compilation succeeded." << std::endl;
        //     } else {
        //         std::cerr << info_log(gs) << std::endl;
        //         throw gl::CompilationFailure();
        //     }
        //     gl::attach(program, gs);
        // }

        auto fs = gl::FragmentShader();
        {
            source(fs, file(root + "src/shader/texture.fs"));
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

    auto texture = gl::Texture2();
    {
        stbi_set_flip_vertically_on_load(true);  

        int width, height, n;
        auto data = stbi_load(
            (root + "/data/wavefront/spot/spot_texture.png").c_str(),
            &width, &height, &n, 3);
        
        gl::active_texture(0);
        gl::bind(texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB,
            width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        gl::throw_if_error();

        stbi_image_free(data);
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    try {
        auto skybox = Skybox({
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, root + "/data/cubemap/canyons/right.jpg"},
            {GL_TEXTURE_CUBE_MAP_POSITIVE_X, root + "/data/cubemap/canyons/left.jpg"},
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, root + "/data/cubemap/canyons/down.jpg"},
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, root + "/data/cubemap/canyons/up.jpg"},
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, root + "/data/cubemap/canyons/bw.jpg"},
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, root + "/data/cubemap/canyons/fw.jpg"}});
    } catch(...) {
        std::cerr << "Skybox failed." << std::endl;
    }

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
            gl::active_texture(0);
            gl::bind(texture);
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
