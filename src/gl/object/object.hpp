#pragma once

#include "traits.hpp"

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/exception/invariant_violation.hpp"

#include <glad/glad.h>

#include <utility>

namespace tlw {
namespace gl {

template<ObjectTraits Traits>
class Object {
public:
    using traits_type = Traits;

    Object() {
        throw_if_error();
    };

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

    ~Object() {
        Traits::delete_(name_);
    }

    GLuint name() const {
        if(!Traits::is(name_)) {
            throw InvariantViolation();
        }
        return name_;
    }

    operator GLuint() const {
        return name();
    }
    

private:
    GLuint name_ = Traits::create();
};

}}
