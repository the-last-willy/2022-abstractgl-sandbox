#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/filesystem/recursive_path.hpp"

#include <agl/standard/all.hpp>

namespace glsl {

class SolidRenderer {
public:
    gl::ProgramObj program;

    gl::OptAttribLoc normal;
    gl::OptAttribLoc position;
    gl::OptAttribLoc texcoords0;

    gl::OptUniformLoc object_to_clip;
    gl::OptUniformLoc object_to_world_normal;
    gl::OptUniformLoc object_to_world_position;

    SolidRenderer() {}
};

inline
SolidRenderer solid_renderer() {
    auto sr = SolidRenderer();
    { // Program.
        { // Compiling and linking.
            auto vertex_shader = gl::VertexShaderObj();
            gl::ShaderSource(vertex_shader,
                agl::standard::string(
                    filesystem::recursive_parent_path(
                        "src/common/glsl/solid_renderer/shader.vert")));
            glCompileShader(vertex_shader);

            auto fragment_shader = gl::Shader(gl::FRAGMENT_SHADER);
            gl::ShaderSource(fragment_shader,
                agl::standard::string(
                    filesystem::recursive_parent_path(
                        "src/common/glsl/solid_renderer/shader.frag")));
            glCompileShader(fragment_shader);

            gl::AttachShader(sr.program, vertex_shader);
            gl::AttachShader(sr.program, fragment_shader);
            gl::LinkProgram(sr.program);
        }
        { // Interface.
            sr.normal = gl::GetAttribLocation(sr.program,
                "a_normal");
            sr.position = gl::GetAttribLocation(sr.program,
                "a_position");
            sr.texcoords0 = gl::GetAttribLocation(sr.program,
                "a_texcoords0");

            sr.object_to_clip = gl::GetUniformLocation(sr.program,
                "object_to_clip");
            sr.object_to_world_normal = gl::GetUniformLocation(sr.program,
                "object_to_world_normal");
            sr.object_to_world_position = gl::GetUniformLocation(sr.program,
                "object_to_world_position");
        }
    }
    return sr;
}

}
