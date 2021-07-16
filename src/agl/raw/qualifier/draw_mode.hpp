#pragma once

#include <glad/glad.h>

namespace agl {

enum class DrawMode : GLenum {
    lines = GL_LINES,
    lines_adjacency = GL_LINES_ADJACENCY,
    line_loop = GL_LINE_LOOP,
    line_strip = GL_LINE_STRIP,
    line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
    patches = GL_PATCHES,
    triangles = GL_TRIANGLES,
    triangles_adjacency = GL_TRIANGLES_ADJACENCY,
    triangle_fan = GL_TRIANGLE_FAN,
    triangle_strip = GL_TRIANGLE_STRIP,
    triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    points = GL_POINTS,
};

}
