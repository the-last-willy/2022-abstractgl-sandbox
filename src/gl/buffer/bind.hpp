#pragma once

#include "buffer.hpp"

namespace tlw {
namespace gl {

template<typename Traits>
void bind_to_array(const Buffer<Traits>& b) {
    glBindBuffer(GL_ARRAY_BUFFER, b);
}

inline
void unbind_array_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename Traits>
void bind_to_element_array(const Buffer<Traits>& b) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b);
}

}}
