#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"

struct WireAxes {
    gl::Buffer colors;
    gl::Buffer positions;

    GLenum mode = GL_LINES;
    GLint first = 0;
    GLsizei count = 6;

    WireAxes() {
        gl::NamedBufferStorage(colors,
            {
                // X.
                glm::vec3(1.f, 0.f, 0.f),
                glm::vec3(1.f, 0.f, 0.f),
                // Y.
                glm::vec3(0.f, 1.f, 0.f),
                glm::vec3(0.f, 1.f, 0.f),
                // Z.
                glm::vec3(0.f, 0.f, 1.f),
                glm::vec3(0.f, 0.f, 1.f)
            },
            GL_NONE);
        gl::NamedBufferStorage(positions,
            {
                // X.
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(1.f, 0.f, 0.f),
                // Y.
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(0.f, 1.f, 0.f),
                // Z.
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(0.f, 0.f, 1.f)
            },
            GL_NONE);
    }
};
