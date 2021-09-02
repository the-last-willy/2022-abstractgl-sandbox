#pragma once

#include "buffer.hpp"

#include <agl/all.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace eng {

struct Accessor {
    std::size_t count = {};

    agl::Offset<GLuint> byte_offset = agl::Offset<GLuint>(0);
    agl::Size<GLint> component_count = agl::Size<GLint>(0);
    int component_size = 0; // In bytes.
    GLenum component_type = GL_NONE;
    agl::Normalized normalized;

    std::vector<float> min = {};
    std::vector<float> max = {};

    // Buffer view.

    agl::Stride<GLsizei> buffer_view_byte_stride = agl::Stride<GLsizei>(0);
    agl::Offset<GLintptr> buffer_view_byte_offset = agl::Offset<GLintptr>(0);

    // Buffer.

    std::shared_ptr<eng::Buffer> buffer = {};
};

template<typename T>
const T& at(const Accessor& a, std::size_t i) {
    auto& data = a.buffer->data;
    auto offset = static_cast<std::size_t>(a.buffer_view_byte_offset)
    + static_cast<std::size_t>(a.byte_offset)
    + i * static_cast<std::size_t>(a.buffer_view_byte_stride);
    return reinterpret_cast<const T&>(data[offset]);
}

template<typename T>
std::optional<T> min(const Accessor& a) {
    if(empty(a.min)) {
        return std::nullopt;
    } else {
        return *reinterpret_cast<const T*>(data(a.min));
    }
}

template<typename T>
std::optional<T> max(const Accessor& a) {
    if(empty(a.max)) {
        return std::nullopt;
    } else {
        return *reinterpret_cast<const T*>(data(a.max));
    }
}

}
