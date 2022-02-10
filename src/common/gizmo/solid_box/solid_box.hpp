#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"

namespace gizmo {

struct SolidBox {
    gl::Buffer element_buffer;
    gl::Buffer normal_buffer;
    gl::Buffer position_buffer;
};

inline
SolidBox solid_box() {
    auto sb = SolidBox();
    gl::NamedBufferStorage(sb.element_buffer,
        {
            // -X.
             0,  1,  2, 
             1,  3,  2,
            // -Y.
             4,  6,  5, 
             5,  6,  7,
            // -Z.
             8,  9, 10, 
             9, 11, 10,
            // +Z.
            12, 14, 13,
            13, 14, 15,
            // +Y.
            16, 17, 18,
            17, 19, 18,
            // +X.
            20, 22, 21,
            21, 22, 23
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
