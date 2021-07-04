#pragma once

#include "gl/all.hpp"
#include "vector/all.hpp"
#include "file.hpp"
#include "root.hpp"

#include <stb_image.h>

#include <map>
#include <string>

namespace tlw {

class Skybox {
public:
    Skybox(std::map<GLenum, std::string> images) {
        // Program.
        {
            auto vs = gl::VertexShader();
            {
                source(vs, file(root + "data/shader/skybox.vs"));
                auto success = gl::try_compile(vs);
                if(success) {
                    std::cout << "-- Skybox vertex shader compilation succeeded." << std::endl;
                } else {
                    std::cerr << info_log(vs) << std::endl;
                    throw gl::CompilationFailure();
                }
                gl::attach(program, vs);
            }
            auto fs = gl::FragmentShader();
            {
                source(fs, file(root + "data/shader/skybox.fs"));
                auto success = gl::try_compile(fs);
                if(success) {
                    std::cout << "-- Skybox fragment shader compilation succeeded." << std::endl;
                } else {
                    std::cerr << info_log(fs) << std::endl;
                    throw gl::CompilationFailure();
                }
                gl::attach(program, fs);
            }
            {
                auto success = gl::try_link(program);
                if(success) {
                    std::cout << "-- Skybox program linkage succeeded." << std::endl;
                } else {
                    std::cerr << gl::info_log(program) << std::endl;
                    throw gl::LinkageFailure();
                }
            }
        }
        // Texture.
        {
            gl::active_texture(0);
            gl::bind(texture);
            for(const auto& [face, filepath] : images) {
                stbi_set_flip_vertically_on_load(true);  

                int width, height, n;
                auto data = stbi_load(filepath.c_str(), &width, &height, &n, 3);

                glTexImage2D(
                    face, 0, GL_RGB, 
                    width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

                    stbi_image_free(data);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        // Vertices.
        {
            gl::bind(vertices);

            gl::bind_to_array(positions);

            Vec2 vertex_positions[] = {
                Vec2{-1.f, -1.f}, Vec2{-1.f,  1.f}, Vec2{ 1.f, -1.f},
                Vec2{ 1.f,  1.f}, Vec2{-1.f,  1.f}, Vec2{1.f, -1.f}
            };

            gl::storage(positions, std::span(vertex_positions));
        }
    }

    gl::VertexArray vertices = {};
    gl::Buffer positions = {};
    gl::Program program = {};
    gl::CubeMap texture = {};
};

}
