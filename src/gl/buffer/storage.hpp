#pragma once

#include "buffer.hpp"

#include <span>
#include <stdexcept>
#include <iostream>

namespace tlw {
namespace gl {

template<typename Type, std::size_t Size>
void storage(const Buffer& b, const std::span<Type, Size>& s) {
    auto size = static_cast<GLsizeiptr>(sizeof(Type) * s.size());
    glNamedBufferStorage(b, size, s.data(), 0);
    {
        auto e = glGetError();
        if(e != GL_NO_ERROR) {
            switch(e) {
            case GL_INVALID_VALUE:
                throw std::runtime_error("Storage size must be greater than 0.");
            case GL_OUT_OF_MEMORY:
                throw std::runtime_error("OpenGL out of memory.");
            }
        }
    }
}

}}
