
#include "gl/all.hpp"
#include "matrix/all.hpp"
#include "scene/all.hpp"
#include "vector/all.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

// External libraries.

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
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

int throwing_main() {
    GLFWwindow* window;
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    
        window = glfwCreateWindow(1280, 720, "Procedural Terrain Generation", NULL, NULL);
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

    auto terrain_size = 2048;

    auto quad_positions = gl::Buffer();
    {
        auto vertices = std::array{
            Vec2{-1.f, -1.f}, Vec2{ 1.f, -1.f}, Vec2{-1.f,  1.f},
            Vec2{ 1.f,  1.f}, Vec2{-1.f,  1.f}, Vec2{ 1.f, -1.f}};
        gl::storage(quad_positions, std::span(vertices));
    }

    auto quad = gl::VertexArray();
    {
        glVertexArrayVertexBuffer( 
            quad, 0, quad_positions, 0, sizeof(Vec2));
        gl::throw_if_error();

        glVertexArrayAttribBinding(
            quad, 0, 0);
        gl::throw_if_error();

        glVertexArrayAttribFormat(
            quad, 0, 2, GL_FLOAT, GL_FALSE, 0);
        gl::throw_if_error();

        glEnableVertexArrayAttrib(quad, 0);
        gl::throw_if_error();
    }
    
    auto height_texture = gl::Texture2();
    {
        glTextureStorage2D(height_texture, 1, GL_R32F, terrain_size, terrain_size);
        gl::throw_if_error();
        glTextureParameteri(height_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(height_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl::throw_if_error();
    }

    auto frame_buffer = gl::FrameBuffer();
    {
        glNamedFramebufferTexture(
            frame_buffer, GL_COLOR_ATTACHMENT0, height_texture, 0);
        gl::throw_if_error();
        gl::throw_if_incomplete(frame_buffer);
    }

    auto terrain_generator = gl::Program();
    {
        auto vs = gl::VertexShader();
        {
            gl::source(vs, file(root + "src/shader/terrain/generator.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(terrain_generator, vs);
        }

        auto fs = gl::FragmentShader();
        {
            gl::source(fs, file(root + "src/shader/terrain/generator.fs"));
            auto success = gl::try_compile(fs);
            if(success) {
                std::cout << "-- Fragment shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(fs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(terrain_generator, fs);
        }

        {
            auto success = gl::try_link(terrain_generator);
            if(success) {
                std::cout << "-- Program linkage succeeded." << std::endl;
            } else {
                std::cerr << gl::info_log(terrain_generator) << std::endl;
                throw gl::LinkageFailure();
            }
        }
    }

    {
        glViewport(0, 0, terrain_size, terrain_size);
        gl::use(terrain_generator);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        gl::bind(quad);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        gl::throw_if_error();
    }

    auto terrain_coords = gl::Buffer();
    {
        glNamedBufferStorage(
            terrain_coords, terrain_size * terrain_size * sizeof(Vec2), NULL, 
            GL_MAP_WRITE_BIT);
        gl::throw_if_error();

        auto mapping = reinterpret_cast<Vec2*>(
            glMapNamedBuffer(terrain_coords, GL_WRITE_ONLY));
        gl::throw_if_error();

        for(int j = 0; j < terrain_size; ++j)
        for(int i = 0; i < terrain_size; ++i) {
            mapping[terrain_size * j + i] = Vec2{
                static_cast<float>(i),
                static_cast<float>(j)};
        }

        glUnmapNamedBuffer(terrain_coords);
    }

    auto terrain_elements = gl::Buffer();
    {
        glNamedBufferStorage(
            terrain_elements, 2 * (terrain_size - 1) * (terrain_size - 1) * sizeof(Vec3i), NULL, 
            GL_MAP_WRITE_BIT);
        gl::throw_if_error();

        auto mapping = reinterpret_cast<Vec3i*>(
            glMapNamedBuffer(terrain_elements, GL_WRITE_ONLY));
        gl::throw_if_error();

        for(int j = 0; j < (terrain_size - 1); ++j)
        for(int i = 0; i < (terrain_size - 1); ++i) {
            // Lower triangle.
            mapping[2 * (j * (terrain_size - 1) + i) + 0] = {
                (j + 0) * (terrain_size) + (i + 0),
                (j + 1) * (terrain_size) + (i + 0),
                (j + 0) * (terrain_size) + (i + 1)};
            // Upper triangle.
            mapping[2 * (j * (terrain_size - 1) + i) + 1] = {
                (j + 1) * (terrain_size) + (i + 1),
                (j + 0) * (terrain_size) + (i + 1),
                (j + 1) * (terrain_size) + (i + 0)};
        }

        glUnmapNamedBuffer(terrain_elements);
    }

    auto terrain_heights = gl::Buffer();
    {
        glNamedBufferStorage(
            terrain_heights, terrain_size * terrain_size * sizeof(GLfloat), NULL, 
            GL_MAP_WRITE_BIT);
        gl::throw_if_error();

        auto mapping = glMapNamedBuffer(terrain_heights, GL_WRITE_ONLY);
        gl::throw_if_error();

        glGetTextureImage(
            height_texture, 0, GL_RED, GL_FLOAT, terrain_size * terrain_size * sizeof(float),
            mapping);
        gl::throw_if_error();

        glUnmapNamedBuffer(terrain_heights);
    }

    auto terrain = gl::VertexArray();
    {
        {
            gl::bind(terrain);
            gl::bind_to_element_array(terrain_elements);
        }
        
        {
            glVertexArrayVertexBuffer( 
                terrain, 0, terrain_coords, 0, sizeof(Vec2));
            gl::throw_if_error();

            glVertexArrayAttribBinding(
                terrain, 0, 0);
            gl::throw_if_error();

            glVertexArrayAttribFormat(
                terrain, 0, 2, GL_FLOAT, GL_FALSE, 0);
            gl::throw_if_error();

            glEnableVertexArrayAttrib(terrain, 0);
            gl::throw_if_error();
        }

        {
            glVertexArrayVertexBuffer( 
                terrain, 1, terrain_heights, 0, sizeof(GLfloat));
            gl::throw_if_error();

            glVertexArrayAttribBinding(
                terrain, 1, 1);
            gl::throw_if_error();

            glVertexArrayAttribFormat(
                terrain, 1, 1, GL_FLOAT, GL_FALSE, 0);
            gl::throw_if_error();

            glEnableVertexArrayAttrib(terrain, 1);
            gl::throw_if_error();
        }
    }

    auto terrain_renderer = gl::Program();
    {
        auto vs = gl::VertexShader();
        {
            gl::source(vs, file(root + "src/shader/terrain/renderer.vs"));
            auto success = gl::try_compile(vs);
            if(success) {
                std::cout << "-- Vertex shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(vs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(terrain_renderer, vs);
        }

        auto gs = gl::GeometryShader();
        {
            gl::source(gs, file(root + "src/shader/terrain/renderer.gs"));
            auto success = gl::try_compile(gs);
            if(success) {
                std::cout << "-- Geometry shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(gs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(terrain_renderer, gs);
        }

        auto fs = gl::FragmentShader();
        {
            gl::source(fs, file(root + "src/shader/terrain/renderer.fs"));
            auto success = gl::try_compile(fs);
            if(success) {
                std::cout << "-- Fragment shader compilation succeeded." << std::endl;
            } else {
                std::cerr << info_log(fs) << std::endl;
                throw gl::CompilationFailure();
            }
            gl::attach(terrain_renderer, fs);
        }

        {
            auto success = gl::try_link(terrain_renderer);
            if(success) {
                std::cout << "-- Program linkage succeeded." << std::endl;
            } else {
                std::cerr << gl::info_log(terrain_generator) << std::endl;
                throw gl::LinkageFailure();
            }
        }
    }
    auto camera = scene::Camera();
    camera.pitch = 0.5f; 
    camera.position = Vec3{0.f, 100.f, 0.f};

    while(!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(glfwGetKey(window, GLFW_KEY_A)) {
            camera.yaw += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_D))
        {
            camera.yaw -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN)) {
            camera.pitch += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            camera.position = camera.position
            + xyz(rotation_y(camera.yaw) * Vec4{0.f, 0.f, -1.f, 1.f});
        }
        if(glfwGetKey(window, GLFW_KEY_UP)) {
            camera.pitch -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            camera.position = camera.position
            + xyz(rotation_y(camera.yaw) * Vec4{0.f, 0.f, 1.f, 1.f});
        }

        {
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);

            auto v = transformation(camera);
            auto p = perspective(camera);

            auto mvp = p * v;

            gl::use(terrain_renderer);
            gl::try_uniform(
                    terrain_renderer, "mvp", mvp);
            
            gl::bind(terrain);
            gl::draw_elements(
                GL_TRIANGLES,
                2 * 3 * (terrain_size - 1) * (terrain_size - 1),
                GL_UNSIGNED_INT, 0);
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
