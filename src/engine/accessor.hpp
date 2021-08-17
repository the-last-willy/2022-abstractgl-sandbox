#pragma once

#include <agl/all.hpp>

namespace eng {

struct Accessor {
    agl::Offset<GLuint> byte_offset = agl::Offset<GLuint>(0);
    agl::Size<GLint> component_count = agl::Size<GLint>(0);
    int component_size = 0; // In bytes.
    GLenum component_type = 0; 
    agl::Normalized normalized;

    // Buffer view.

    agl::Stride<GLsizei> buffer_view_byte_stride = agl::Stride<GLsizei>(0);
    agl::Offset<GLintptr> buffer_view_byte_offset = agl::Offset<GLintptr>(0);

    // Buffer.

    agl::Buffer buffer = agl::none;
};

}
