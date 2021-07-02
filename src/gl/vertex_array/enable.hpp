#pragma once

#include "vertex_array.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

inline
void enable(const VertexArray& va, GLuint index) {
    glEnableVertexArrayAttrib(va, index);
    {
        auto e = glGetError();
        if(e != GL_NO_ERROR) {
            switch(e) {
            case GL_INVALID_OPERATION:
                throw std::runtime_error(
                    "Vertex array: invalid operation.");
            case GL_INVALID_VALUE:
                throw std::runtime_error(
                    "Vertex array: invalid value.");
            }
        }
    }
}

}}
