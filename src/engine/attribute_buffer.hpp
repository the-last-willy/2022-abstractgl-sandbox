#pragma once

#include <agl/all.hpp>

struct AttributeBuffer {
    agl::Buffer buffer = agl::buffer();

    agl::Offset<GLintptr> offset = agl::Offset<GLintptr>(0);
    agl::Size<GLint> size = agl::Size<GLint>(0);
    agl::Stride<GLsizei> stride = agl::Stride<GLsizei>(0);
    GLenum type = 0;
};
