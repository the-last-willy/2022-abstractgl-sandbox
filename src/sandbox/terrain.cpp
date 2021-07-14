
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

int throwing_main() {
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
    
        glfwSetKeyCallback(window, key_callback);
    
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

    auto terrain_size = 1 << (9 - 1);

    auto quad_positions = gl::Buffer<Vec2>();
    {
        auto positions = std::array{
            Vec2{-1.f, -1.f}, Vec2{ 1.f, -1.f}, Vec2{-1.f,  1.f},
            Vec2{ 1.f,  1.f}, Vec2{-1.f,  1.f}, Vec2{ 1.f, -1.f}};
        quad_positions = gl::Buffer(std::span(positions));
    }

    auto terrain_generator = gl::program({
        {
            GL_VERTEX_SHADER,
            file(root + "src/shader/terrain/generator.vs").c_str()
        },
        {
            GL_FRAGMENT_SHADER,
            file(root + "src/shader/terrain/generator.fs").c_str()
        }
    });

    auto quad = gl::VertexArray();
    {
        glVertexArrayVertexBuffer( 
            quad, 0, quad_positions, 0, stride(quad_positions));
        glVertexArrayAttribFormat(
            quad, 0, 2, GL_FLOAT, GL_FALSE, 0);

        {
            auto l = gl::AttributeLocation(terrain_generator, "position");
            glVertexArrayAttribBinding(quad, l, 0);
            glEnableVertexArrayAttrib(quad, l);
        }
    }

    auto color_texture = gl::Texture2();
    {
        glTextureStorage2D(color_texture, 1, GL_RGBA8, terrain_size, terrain_size);
        gl::throw_if_error();
        glTextureParameteri(color_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(color_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    auto normal_texture = gl::Texture2();
    {
        glTextureStorage2D(normal_texture, 1, GL_RGB32F, terrain_size, terrain_size);
        gl::throw_if_error();
        glTextureParameteri(normal_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(normal_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl::throw_if_error();
    }

    auto frame_buffer = gl::FrameBuffer();
    {
        glNamedFramebufferTexture(
            frame_buffer, GL_COLOR_ATTACHMENT0, color_texture, 0);
        gl::throw_if_error();
        glNamedFramebufferTexture(
            frame_buffer, GL_COLOR_ATTACHMENT1, height_texture, 0);
        gl::throw_if_error();
        glNamedFramebufferTexture(
            frame_buffer, GL_COLOR_ATTACHMENT2, normal_texture, 0);
        gl::throw_if_error();
        gl::throw_if_incomplete(frame_buffer);

        {
            auto buffers = std::array<GLenum, 3>{
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2};
            glNamedFramebufferDrawBuffers(
                frame_buffer, GLsizei(size(buffers)), data(buffers));
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

    auto terrain_coords = gl::Buffer<Vec2>();
    {
        terrain_coords = gl::Buffer<Vec2>(
            terrain_size * terrain_size,
            GL_MAP_WRITE_BIT);

        auto mapping = gl::BufferMapping(terrain_coords, GL_WRITE_ONLY);

        for(int j = 0; j < terrain_size; ++j)
        for(int i = 0; i < terrain_size; ++i) {
            mapping[terrain_size * j + i] = Vec2{
                static_cast<float>(i),
                static_cast<float>(j)};
        }
    }

    auto terrain_elements = gl::Buffer<Vec3i>();
    {
        terrain_elements = gl::Buffer<Vec3i>(
            2 * (terrain_size - 1) * (terrain_size - 1), // Number of triangles.
            GL_MAP_WRITE_BIT);

        auto mapping = gl::BufferMapping(terrain_elements, GL_WRITE_ONLY);

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
    }

    auto terrain_heights = gl::Buffer<GLfloat>(
        terrain_size * terrain_size,
        GL_MAP_WRITE_BIT);
    {
        auto mapping = gl::BufferMapping(terrain_heights, GL_WRITE_ONLY);

        // Undefined behaviour.
        glGetTextureImage(
            height_texture, 0, GL_RED, GL_FLOAT, terrain_size * terrain_size * sizeof(float),
            mapping.data());
        gl::throw_if_error();
    }

    auto terrain_normals = gl::Buffer<Vec3>(
        terrain_size * terrain_size,
        GL_MAP_WRITE_BIT);
    {
        auto mapping = gl::BufferMapping(terrain_normals, GL_WRITE_ONLY);

        // Undefined behaviour.
        glGetTextureImage(
            normal_texture, 0, GL_RGB, GL_FLOAT, terrain_size * terrain_size * sizeof(Vec3),
            mapping.data());
        gl::throw_if_error();
    }

    auto terrain_uvs = gl::Buffer<Vec2>(
        terrain_size * terrain_size,
        GL_MAP_WRITE_BIT);
    {
        auto mapping = gl::BufferMapping(terrain_uvs, GL_WRITE_ONLY);

        for(int j = 0; j < terrain_size; ++j)
        for(int i = 0; i < terrain_size; ++i) {
            mapping[j * terrain_size + i] = Vec2{
                float(i) / (terrain_size - 1),
                float(j) / (terrain_size - 1)};
        }
    }

    auto terrain_renderer = gl::program({
        {
            GL_VERTEX_SHADER,
            file(root + "src/shader/terrain/renderer.vs").c_str()
        },
        {
            GL_FRAGMENT_SHADER,
            file(root + "src/shader/terrain/renderer.fs").c_str()
        }
    });

    auto terrain = gl::VertexArray();
    {
        {
            gl::bind(terrain);
            gl::bind_to_element_array(terrain_elements);
        }
        
        {
            glVertexArrayVertexBuffer( 
                terrain, 0, terrain_coords, 0, stride(terrain_coords));
            glVertexArrayAttribFormat(
                terrain, 0, 2, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(terrain_renderer, "coords");
                glVertexArrayAttribBinding(terrain, l, 0);
                glEnableVertexArrayAttrib(terrain, l);
            }
        }

        {
            glVertexArrayVertexBuffer( 
                terrain, 1, terrain_heights, 0, stride(terrain_heights));
            glVertexArrayAttribFormat(
                terrain, 1, 1, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(terrain_renderer, "height");
                glVertexArrayAttribBinding(terrain, l, 1);
                glEnableVertexArrayAttrib(terrain, l);
            }
        }

        {
            glVertexArrayVertexBuffer( 
                terrain, 2, terrain_normals, 0, stride(terrain_normals));
            glVertexArrayAttribFormat(
                terrain, 2, 3, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(terrain_renderer, "normal");
                glVertexArrayAttribBinding(terrain, l, 2);
                glEnableVertexArrayAttrib(terrain, l);
            }
        }

        {
            glVertexArrayVertexBuffer( 
                terrain, 3, terrain_uvs, 0, stride(terrain_uvs));
            glVertexArrayAttribFormat(
                terrain, 3, 2, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(terrain_renderer, "uv");
                glVertexArrayAttribBinding(
                    terrain, l, 3);
                glEnableVertexArrayAttrib(terrain, l);
            }
        }
    }

    auto normal_renderer = gl::program({
        {
            GL_VERTEX_SHADER,
            file(root + "src/shader/debug/normal_lines.vs").c_str()
        },
        {
            GL_GEOMETRY_SHADER,
            file(root + "src/shader/debug/normal_lines.gs").c_str()
        },
        {
            GL_FRAGMENT_SHADER,
            file(root + "src/shader/debug/normal_lines.fs").c_str()
        }
    });

    auto terrain_debug = gl::VertexArray();
    {
        {
            gl::bind(terrain_debug);
            gl::bind_to_element_array(terrain_elements);
        }
        
        {
            glVertexArrayVertexBuffer( 
                terrain_debug, 0, terrain_coords, 0, stride(terrain_coords));
            glVertexArrayAttribFormat(
                terrain_debug, 0, 2, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(normal_renderer, "coords");
                glVertexArrayAttribBinding(terrain_debug, l, 0);
                glEnableVertexArrayAttrib(terrain_debug, l);
            }
        }

        {
            glVertexArrayVertexBuffer( 
                terrain_debug, 1, terrain_heights, 0, sizeof(GLfloat));
            glVertexArrayAttribFormat(
                terrain_debug, 1, 1, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(normal_renderer, "height");
                glVertexArrayAttribBinding(terrain_debug, l, 1);
                glEnableVertexArrayAttrib(terrain_debug, l);
            } 
        }

        {
            glVertexArrayVertexBuffer( 
                terrain_debug, 2, terrain_normals, 0, stride(terrain_normals));
            glVertexArrayAttribFormat(
                terrain_debug, 2, 3, GL_FLOAT, GL_FALSE, 0);

            {
                auto l = gl::AttributeLocation(normal_renderer, "normal");
                glVertexArrayAttribBinding(terrain_debug, l, 2);
                glEnableVertexArrayAttrib(terrain_debug, l);
            }
        }
    }

    glActiveTexture(GL_TEXTURE0);
    gl::bind(color_texture);

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

            auto mv = v;
            auto mvp = p * mv;

            {
                gl::use(terrain_renderer);
                gl::bind(terrain);

                gl::Uniform(terrain_renderer, "light_direction")
                    = normalized(Vec3{-1.f, -1.f, 0.f});
                gl::Uniform(terrain_renderer, "mv") = mv;
                gl::Uniform(terrain_renderer, "mvp") = mvp;
                gl::Uniform(terrain_renderer, "tex") = 0;
                
                gl::draw_elements(
                    GL_TRIANGLES,
                    2 * 3 * (terrain_size - 1) * (terrain_size - 1),
                    GL_UNSIGNED_INT, 0);
            }
            {
                gl::use(normal_renderer);
                gl::bind(terrain_debug);

                gl::Uniform(normal_renderer, "mvp") = mvp;

                gl::draw_elements(
                    GL_TRIANGLES,
                    2 * 3 * (terrain_size - 1) * (terrain_size - 1),
                    GL_UNSIGNED_INT, 0);
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
