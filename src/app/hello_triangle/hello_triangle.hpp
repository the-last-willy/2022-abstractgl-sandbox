#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"
#include "common/opengl/debug_message_callback.hpp"
#include "common/all.hpp"

#include <agl/standard/all.hpp>

struct HelloTriangle {
    WireframeRenderer wireframe_renderer = ::wireframe_renderer();

    WireAxes wire_axes;
    gl::VertexArray wire_axes_wireframe_renderer_vao;


    gl::Program shader_program;
    gl::OptUniformLoc object_to_clip_location;

    gizmo::SolidBox solid_box;
    gl::VertexArray solid_box_vao;

    // Camera.
    glm::mat4 view_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_view = glm::mat4(1.f);
    glm::vec2 yaw_pitch = glm::vec2(0.f, 0.f);
    glm::vec3 camera_position = glm::vec3(0.f);
};

void init(HelloTriangle& _this) {
    std::ignore = _this;
    { // VAOs.
        _this.wire_axes_wireframe_renderer_vao
        = vertex_array(_this.wire_axes, _this.wireframe_renderer);
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
    { // Camera.
        _this.view_to_clip = glm::perspective(
            3.141593f / 2.f,
            16.f / 9.f,
            0.1f,
            100.f);
    }
    { // Solid box.
        _this.solid_box = gizmo::solid_box();
        // { // Normal attribute.
        //     auto attribindex = gl::GetAttribLocation(_this.shader_program, "a_normal");
        //     auto bindingindex = GLuint(0);
        //     gl::VertexArrayAttribFormat(_this.solid_box_vao,
        //         attribindex,
        //         3, GL_FLOAT,
        //         GL_FALSE, 0);
        //     gl::VertexArrayVertexBuffer(_this.solid_box_vao,
        //         bindingindex,
        //         _this.solid_box.normal_buffer,
        //         0, sizeof(glm::vec3));
        //     gl::VertexArrayAttribBinding(_this.solid_box_vao,
        //         attribindex,
        //         bindingindex);
        //     gl::EnableVertexArrayAttrib(_this.solid_box_vao,
        //         attribindex);
        // }
        { // Position attribute.
            auto attribindex = gl::GetAttribLocation(_this.shader_program, "a_position");
            auto bindingindex = GLuint(1);
            gl::VertexArrayAttribFormat(_this.solid_box_vao,
                attribindex,
                3, GL_FLOAT,
                GL_FALSE, 0);
            gl::VertexArrayVertexBuffer(_this.solid_box_vao,
                bindingindex,
                _this.solid_box.position_buffer,
                0, sizeof(glm::vec3));
            gl::VertexArrayAttribBinding(_this.solid_box_vao,
                attribindex,
                bindingindex);
            gl::EnableVertexArrayAttrib(_this.solid_box_vao,
                attribindex);
        }
        { // Element buffer.
            gl::VertexArrayElementBuffer(_this.solid_box_vao,
                _this.solid_box.element_buffer);
        }
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
    gl::ClearNamedFramebuffer(0, GL_DEPTH, 0, 1.f);

    { // Solid renderer.
        glUseProgram(_this.shader_program);
        glBindVertexArray(_this.solid_box_vao);

        glCullFace(GL_FRONT);
        auto cull_cap = scoped(gl::Enable(GL_CULL_FACE));

        glDepthFunc(GL_LESS);
        auto depth_cap = scoped(gl::Enable(GL_DEPTH_TEST));

        glProgramUniformMatrix4fv(_this.shader_program,
            _this.object_to_clip_location,
            1, GL_FALSE,
            &_this.world_to_clip[0][0]);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    { // Wireframe renderer.
        glUseProgram(_this.wireframe_renderer.program);
        
        glBindVertexArray(_this.wire_axes_wireframe_renderer_vao);

        glDepthFunc(GL_LESS);
        auto depth_cap = scoped(gl::Enable(GL_DEPTH_TEST));

        glProgramUniformMatrix4fv(_this.wireframe_renderer.program,
            _this.wireframe_renderer.object_to_clip,
            1, GL_FALSE,
            &_this.world_to_clip[0][0]);

        glDrawArrays(
            _this.wire_axes.mode,
            _this.wire_axes.first,
            _this.wire_axes.count);
    }
}
