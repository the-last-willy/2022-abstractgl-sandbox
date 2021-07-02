#pragma once

#include "gl/object/object.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

class VertexArray : public Object {
public:
    VertexArray() {
        glCreateVertexArrays (1, &name_);
        if(!glIsVertexArray(name_)) {
            throw std::runtime_error(
                "VertexArray: Failed to construct.");
        }
    }

    using Object::Object;

    ~VertexArray() noexcept {
        glDeleteVertexArrays(1, &name_);
    }

    GLuint name() const {
        if(!glIsBuffer(name_)) {
            throw std::runtime_error(
                "VertexArray: Invalid name.");
        }
        return name_;
    }

    operator GLuint() const {
        return name_;
    }
};

}}
