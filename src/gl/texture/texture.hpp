#pragma once

#include "glad/glad.h"

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/exception/invariant_violation.hpp"

namespace tlw {
namespace gl {

class Texture {
public:
    Texture(GLenum target) {
        glCreateTextures(target, 1, &name_);
        throw_if_error();
    }

    Texture(const Texture&) = delete;

    Texture(Texture&&) = delete;

    ~Texture() noexcept {
        glDeleteTextures(1, &name_);
    }

    GLuint name() const {
        if(!glIsTexture(name_)) {
            throw InvariantViolation();
        }
        return name_;
    }

    GLenum target() const {
        
    }

    operator GLuint() const {
        return name();
    }

private:
    GLuint name_ = 0;
};

inline
auto cube_map() {
    return Texture(GL_TEXTURE_CUBE_MAP);
}

inline
auto texture2() {
    return Texture(GL_TEXTURE_2D);
}

}}
