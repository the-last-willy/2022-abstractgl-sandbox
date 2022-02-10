#pragma once

#include "../wire_axes.hpp"
#include "glsl/wireframe_renderer/wireframe_renderer.hpp"

inline
gl::VertexArray vertex_array(const WireAxes& wa, const WireframeRenderer& wr) {
    auto va = gl::VertexArray();
    { // Color.
        auto bindingindex = GLuint(0);
        gl::VertexArrayAttribFormat(va,
            wr.color,
            3, GL_FLOAT,
            GL_FALSE, 0);
        gl::VertexArrayVertexBuffer(va,
            bindingindex,
            wa.colors,
            0, sizeof(glm::vec3));
        gl::VertexArrayAttribBinding(va,
            wr.color,
            bindingindex);
        gl::EnableVertexArrayAttrib(va,
            wr.color);
    }
    { // Position.
        auto bindingindex = GLuint(1);
        gl::VertexArrayAttribFormat(va,
            wr.position,
            3, GL_FLOAT,
            GL_FALSE, 0);
        gl::VertexArrayVertexBuffer(va,
            bindingindex,
            wa.positions,
            0, sizeof(glm::vec3));
        gl::VertexArrayAttribBinding(va,
            wr.position,
            bindingindex);
        gl::EnableVertexArrayAttrib(va,
            wr.position);
    }
    return va;
}
