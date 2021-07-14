#pragma once

#include "gl/program/program.hpp"

namespace tlw {
namespace gl {

struct NoAttributeLocation {};

struct AttributeLocation {
    AttributeLocation() noexcept = default;

    AttributeLocation(const Program& p, const char* name)
        : index(glGetAttribLocation(p, name))
    {}

    operator bool() const noexcept {
        return index != -1;
    }

    operator GLuint() const {
        if(*this) {
            return index;
        } else {
            throw NoAttributeLocation();
        }
    }

    GLint index = -1;
};

}}
