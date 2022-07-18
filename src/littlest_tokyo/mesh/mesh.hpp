#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"

struct Mesh {
    gl::BufferObj indices;
    gl::BufferObj normals;
    gl::BufferObj positions;
    gl::BufferObj texcoords0;

    GLsizei draw_count = 0;
    GLenum draw_mode;
    GLenum draw_type;
};
