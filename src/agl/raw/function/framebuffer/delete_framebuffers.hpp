#pragma once

#include "agl/raw/qualifier/framebuffer.hpp"

#include <span>

namespace agl {

inline
void delete_framebuffers(GLsizei n, Framebuffer* fs) {
    glDeleteFramebuffers(
        n,
        reinterpret_cast<GLuint*>(fs));
}

inline
void delete_(std::span<Framebuffer> s) {
    delete_framebuffers(
        static_cast<GLsizei>(size(s)),
        data(s));
}

inline
void delete_(Framebuffer f) {
    delete_({&f, 1});
}

}
