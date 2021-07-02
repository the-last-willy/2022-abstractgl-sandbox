#pragma once

#include <glad/glad.h>

#include <utility>

namespace tlw {
namespace gl {

class Object {
protected:
    Object() noexcept = default;

    Object(GLuint name) noexcept
        : name_{name}
    {}

    Object(const Object&) = delete;

    Object(Object&& o) noexcept {
        *this = std::move(o);
    };

    Object& operator=(const Object&) = delete;

    Object& operator=(Object&& b) noexcept { 
        name_ = b.name_;
        b.name_ = 0;
        return *this;
    }

    GLuint name() const noexcept {
        return name_;
    }

    GLuint name_ = 0;
};

}}
