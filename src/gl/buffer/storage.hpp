#pragma once

#include "buffer.hpp"

#include "gl/exception/error/throw_if_error.hpp"

#include <span>

namespace tlw {
namespace gl {

template<typename Type, std::size_t Size>
void storage(const Buffer& b, const std::span<Type, Size>& s) {
    auto size = static_cast<GLsizeiptr>(sizeof(Type) * s.size());
    glNamedBufferStorage(b, size, s.data(), 0);
    throw_if_error();
}

}}
