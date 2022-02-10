#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"

#include <agl/standard/all.hpp>

struct WireframeRenderer {
    gl::Program program;

    // Attribute locations.

    gl::AttribLoc color = 0;
    gl::AttribLoc position = 0;

    // Uniform locations.

    gl::OptUniformLoc object_to_clip;
};

inline
WireframeRenderer wireframe_renderer() {
    auto wr = WireframeRenderer();
    { // Program.
        { // Compiling and linking.
            auto vertex_shader = gl::Shader(gl::VERTEX_SHADER);
            gl::ShaderSource(vertex_shader,
                agl::standard::string(
                    "D:/dev/project/abstractgl-sandbox/src/common/glsl/wireframe_renderer/shader.vert"));
            glCompileShader(vertex_shader);

            auto fragment_shader = gl::Shader(gl::FRAGMENT_SHADER);
            gl::ShaderSource(fragment_shader,
                agl::standard::string(
                    "D:/dev/project/abstractgl-sandbox/src/common/glsl/wireframe_renderer/shader.frag"));
            glCompileShader(fragment_shader);

            gl::AttachShader(wr.program, vertex_shader);
            gl::AttachShader(wr.program, fragment_shader);
            gl::LinkProgram(wr.program);
        }
        { // Interface.
            wr.color = gl::GetAttribLocation(wr.program,
                "a_color");
            wr.position = gl::GetAttribLocation(wr.program,
                "a_position");

            wr.object_to_clip = gl::GetUniformLocation(wr.program,
                "object_to_clip");
        }
    }
    return wr;
}
