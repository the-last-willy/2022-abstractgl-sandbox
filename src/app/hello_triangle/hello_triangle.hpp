#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"
#include "common/opengl/debug_message_callback.hpp"

#include <agl/standard/all.hpp>

struct HelloTriangle {

    gl::Buffer color_buffer;
    gl::Buffer position_buffer;

    gl::Program shader_program;
    gl::VertexArray vertex_array;
};

void init(HelloTriangle& ht) {
    std::ignore = ht;
    {
        glDebugMessageCallback(&opengl_debug_message_callback, NULL);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
    { // Shader program.
        auto vertex_shader = gl::Shader(gl::VERTEX_SHADER);
        gl::ShaderSource(vertex_shader,
            agl::standard::string(
                "D:/dev/project/abstractgl-sandbox/src/common/glsl/shader/test.vert"));
        glCompileShader(vertex_shader);

        auto fragment_shader = gl::Shader(gl::FRAGMENT_SHADER);
        gl::ShaderSource(fragment_shader,
            agl::standard::string(
                "D:/dev/project/abstractgl-sandbox/src/common/glsl/shader/test.frag"));
        glCompileShader(fragment_shader);

        gl::AttachShader(ht.shader_program, vertex_shader);
        gl::AttachShader(ht.shader_program, fragment_shader);
        gl::LinkProgram(ht.shader_program);
    }
    { // Triangle vertex array.
        gl::NamedBufferStorage(
            ht.color_buffer,
            {
                glm::vec3(1.f, 0.f, 0.f),
                glm::vec3(0.f, 1.f, 0.f),
                glm::vec3(0.f, 0.f, 1.f)
            },
            GL_NONE);
        gl::NamedBufferStorage(
            ht.position_buffer,
            {
                glm::vec3(-0.5f,  0.0f, 0.f),
                glm::vec3(+0.5f, -0.5f, 0.f),
                glm::vec3(+0.5f, +0.5f, 0.f)
            },
            GL_NONE);
        { // Color attribute.
            auto attribindex = gl::GetAttribLocation(ht.shader_program, "a_color");
            auto bindingindex = GLuint(0);
            gl::VertexArrayAttribFormat(ht.vertex_array,
                attribindex,
                3, GL_FLOAT,
                GL_FALSE, 0);
            gl::VertexArrayVertexBuffer(ht.vertex_array,
                bindingindex,
                ht.color_buffer,
                0, sizeof(glm::vec3));
            gl::VertexArrayAttribBinding(ht.vertex_array,
                attribindex,
                bindingindex);
            gl::EnableVertexArrayAttrib(ht.vertex_array,
                attribindex);
        }
        { // Position attribute.
            auto attribindex = gl::GetAttribLocation(ht.shader_program, "a_position");
            auto bindingindex = GLuint(1);
            gl::VertexArrayAttribFormat(ht.vertex_array,
                attribindex,
                3, GL_FLOAT,
                GL_FALSE, 0);
            gl::VertexArrayVertexBuffer(ht.vertex_array,
                bindingindex,
                ht.position_buffer,
                0, sizeof(glm::vec3));
            gl::VertexArrayAttribBinding(ht.vertex_array,
                attribindex,
                bindingindex);
            gl::EnableVertexArrayAttrib(ht.vertex_array,
                attribindex);
        }
    }
}

void update(HelloTriangle& ht) {
    std::ignore = ht;
}

void render(HelloTriangle& ht) {
    glUseProgram(ht.shader_program);
    glBindVertexArray(ht.vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
