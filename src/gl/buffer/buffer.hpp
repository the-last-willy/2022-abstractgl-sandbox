#pragma once

#include "gl/exception/error/throw_if_error.hpp"

#include <glad/glad.h>

#include <iostream>
#include <span>

namespace tlw {
namespace gl {

template<typename Type>
class Buffer {
public:
    Buffer() noexcept {}

    Buffer(GLsizei size, GLbitfield flags = 0) {
        glCreateBuffers(1, &name_);
        auto byte_count = size * sizeof(Type);
        glNamedBufferStorage(name_, byte_count, NULL, flags);
        throw_if_error();
    }

    Buffer(std::span<Type> elements, GLbitfield flags = 0) {
        glCreateBuffers(1, &name_);
        throw_if_error();
        auto byte_count = static_cast<GLsizei>(size(elements) * sizeof(Type));
        glNamedBufferStorage(name_, byte_count, data(elements), flags);
        throw_if_error();
    }

    Buffer(const Buffer&) = delete;

    Buffer(Buffer&& b) noexcept
        : name(b.name_)
    {
        b.name_ = 0;
        return *this;
    }

    Buffer& operator=(const Buffer&) = delete;

    Buffer& operator=(Buffer&& b) noexcept {
        glDeleteBuffers(1, &name_);
        name_ = b.name_;
        b.name_ = 0;
        return *this;
    }

    ~Buffer() noexcept {
        glDeleteBuffers(1, &name_);
    }

    GLuint name() const {
        return name_;
    }

    operator GLuint() const {
        return name();
    }

private:
    GLuint name_ = 0;
};

template<typename Type, std::size_t Size>
Buffer(std::span<Type, Size>) -> Buffer<Type>;

template<typename Type, std::size_t Size>
Buffer(std::span<Type, Size>, GLbitfield) -> Buffer<Type>;

}}
