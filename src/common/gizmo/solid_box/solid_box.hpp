#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"

namespace gizmo {

struct SolidBox {
    gl::BufferObj element_buffer;
    gl::BufferObj normal_buffer;
    gl::BufferObj position_buffer;
};

inline
SolidBox solid_box() {
    auto sb = SolidBox();
    gl::NamedBufferStorage(sb.element_buffer,
        {
            // -X.
             0,  2,  1, 
             1,  2,  3,
            // -Y.
             4,  5,  6, 
             5,  7,  6,
            // -Z.
             8, 10,  9, 
             9, 10, 11,
            // +Z.
            12, 13, 14,
            13, 15, 14,
            // +Y.
            16, 18, 17,
            17, 18, 19,
            // +X.
            20, 21, 22,
            21, 23, 22
        },
        GL_NONE);
    gl::NamedBufferStorage(sb.normal_buffer,
        {
            // -X.
            glm::vec3(-1.f,  0.f,  0.f),
            glm::vec3(-1.f,  0.f,  0.f),
            glm::vec3(-1.f,  0.f,  0.f),
            glm::vec3(-1.f,  0.f,  0.f),
            // -Y.
            glm::vec3( 0.f, -1.f,  0.f),
            glm::vec3( 0.f, -1.f,  0.f),
            glm::vec3( 0.f, -1.f,  0.f),
            glm::vec3( 0.f, -1.f,  0.f),
            // -Z.
            glm::vec3( 0.f,  0.f, -1.f),
            glm::vec3( 0.f,  0.f, -1.f),
            glm::vec3( 0.f,  0.f, -1.f),
            glm::vec3( 0.f,  0.f, -1.f),
            // +Z.
            glm::vec3( 0.f,  0.f, +1.f),
            glm::vec3( 0.f,  0.f, +1.f),
            glm::vec3( 0.f,  0.f, +1.f),
            glm::vec3( 0.f,  0.f, +1.f),
            // +Y.
            glm::vec3( 0.f, +1.f,  0.f),
            glm::vec3( 0.f, +1.f,  0.f),
            glm::vec3( 0.f, +1.f,  0.f),
            glm::vec3( 0.f, +1.f,  0.f),
            // +X.
            glm::vec3(+1.f,  0.f,  0.f),
            glm::vec3(+1.f,  0.f,  0.f),
            glm::vec3(+1.f,  0.f,  0.f),
            glm::vec3(+1.f,  0.f,  0.f)
        },
        GL_NONE);
    gl::NamedBufferStorage(sb.position_buffer,
        {
            // -X.
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(-0.5f, +0.5f, +0.5f),
            // -Y.
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, -0.5f, +0.5f),
            // -Z.
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            // +Z.
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, -0.5f, +0.5f),
            glm::vec3(-0.5f, +0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f),
            // +Y.
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            glm::vec3(-0.5f, +0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f),
            // +X.
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            glm::vec3(+0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f)
        },
        GL_NONE);
    return sb;
}

}
