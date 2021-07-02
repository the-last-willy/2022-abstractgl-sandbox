#pragma once

#include "gl/object/object.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

class Buffer : public Object {
public:
    Buffer() {
        glCreateBuffers(1, &name_);
    }

    using Object::Object;

    ~Buffer() noexcept {
        glDeleteBuffers(1, &name_);
    }

    GLuint name() const {
        if(!glIsBuffer(name_)) {
            throw std::runtime_error(
                "Buffer: Invalid object name.");
        }
        return name_;
    }

    operator GLuint() const {
        return name_;
    }
};

}}
