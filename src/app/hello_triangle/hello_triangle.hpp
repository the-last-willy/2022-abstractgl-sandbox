#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"
#include "common/opengl/debug_message_callback.hpp"

#include <agl/standard/all.hpp>

struct HelloTriangle {
    gl::Buffer color_buffer;
    gl::Buffer position_buffer;

    gl::Program shader_program;
    gl::OptUniformLoc object_to_clip_location;

    gl::VertexArray vertex_array;

    // Camera.
    glm::mat4 view_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_view = glm::mat4(1.f);
    glm::vec2 yaw_pitch = glm::vec2(0.f, 0.f);
    glm::vec3 camera_position = glm::vec3(0.f);
};

void init(HelloTriangle& _this) {
    std::ignore = _this;
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

        gl::AttachShader(_this.shader_program, vertex_shader);
        gl::AttachShader(_this.shader_program, fragment_shader);
        gl::LinkProgram(_this.shader_program);

        _this.object_to_clip_location = gl::GetUniformLocation(
            _this.shader_program,
            "object_to_clip");
    }
    { // Triangle vertex array.
        gl::NamedBufferStorage(
            _this.color_buffer,
            {
                glm::vec3(1.f, 0.f, 0.f),
                glm::vec3(0.f, 1.f, 0.f),
                glm::vec3(0.f, 0.f, 1.f)
            },
            GL_NONE);
        gl::NamedBufferStorage(
            _this.position_buffer,
            {
                glm::vec3(-0.5f,  0.0f, 0.f),
                glm::vec3(+0.5f, -0.5f, 0.f),
                glm::vec3(+0.5f, +0.5f, 0.f)
            },
            GL_NONE);
        { // Color attribute.
            auto attribindex = gl::GetAttribLocation(_this.shader_program, "a_color");
            auto bindingindex = GLuint(0);
            gl::VertexArrayAttribFormat(_this.vertex_array,
                attribindex,
                3, GL_FLOAT,
                GL_FALSE, 0);
            gl::VertexArrayVertexBuffer(_this.vertex_array,
                bindingindex,
                _this.color_buffer,
                0, sizeof(glm::vec3));
            gl::VertexArrayAttribBinding(_this.vertex_array,
                attribindex,
                bindingindex);
            gl::EnableVertexArrayAttrib(_this.vertex_array,
                attribindex);
        }
        { // Position attribute.
            auto attribindex = gl::GetAttribLocation(_this.shader_program, "a_position");
            auto bindingindex = GLuint(1);
            gl::VertexArrayAttribFormat(_this.vertex_array,
                attribindex,
                3, GL_FLOAT,
                GL_FALSE, 0);
            gl::VertexArrayVertexBuffer(_this.vertex_array,
                bindingindex,
                _this.position_buffer,
                0, sizeof(glm::vec3));
            gl::VertexArrayAttribBinding(_this.vertex_array,
                attribindex,
                bindingindex);
            gl::EnableVertexArrayAttrib(_this.vertex_array,
                attribindex);
        }
    }
    { // Camera.
        _this.view_to_clip = glm::perspective(
            3.141593f / 2.f,
            16.f / 9.f,
            0.1f,
            100.f);
    }
}

void update(HelloTriangle& _this) {
    auto& io = ImGui::GetIO();
    if(not io.WantCaptureKeyboard) {
        auto forward = glm::vec3(inverse(_this.world_to_view) * glm::vec4(0.f, 0.f, -1.f, 0.f));
        auto right = glm::vec3(inverse(_this.world_to_view) * glm::vec4(+1.f, 0.f, 0.f, 0.f));
        if(ImGui::IsKeyDown('A')) {
            _this.camera_position += right / 30.f;
        }
        if(ImGui::IsKeyDown('D')) {
            _this.camera_position -= right / 30.f;
        }
        if(ImGui::IsKeyDown('S')) {
            _this.camera_position += forward / 30.f;
        }
        if(ImGui::IsKeyDown('W')) {
            _this.camera_position -= forward / 30.f;
        }
    }
    if(not io.WantCaptureMouse) {
        if(ImGui::IsMouseDown(0)) {
            _this.yaw_pitch += glm::vec2(io.MouseDelta[0], io.MouseDelta[1]) / 100.f;
        }
    }
    { // Camera.
        _this.world_to_view = glm::translate(
            glm::rotate(
                glm::rotate(
                    glm::mat4(1.f),
                    _this.yaw_pitch.y,
                    glm::vec3(1.f, 0.f, 0.f)),
                _this.yaw_pitch.x,
                glm::vec3(0.f, 1.f, 0.f)),
            _this.camera_position);
        _this.world_to_clip = _this.view_to_clip * _this.world_to_view;
    }
}

void render(HelloTriangle& _this) {
    glUseProgram(_this.shader_program);
    glBindVertexArray(_this.vertex_array);

    glProgramUniformMatrix4fv(_this.shader_program,
        _this.object_to_clip_location,
        1, GL_FALSE,
        &_this.world_to_clip[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
