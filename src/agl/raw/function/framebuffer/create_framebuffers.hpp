#pragma once

#include "agl/raw/qualifier/framebuffer.hpp"

#include <span>

namespace agl {

inline
void create_framebuffers(GLsizei n, Framebuffer* fs) {
    glCreateFramebuffers(
        n,
        reinterpret_cast<GLuint*>(fs));
}

inline
void create(std::span<Framebuffer> s) {
    create_framebuffers(
        static_cast<GLsizei>(size(s)),
        data(s));
}

inline
Framebuffer framebuffer() {
    auto f = Framebuffer();
    create({&f, 1});
    return f;
}

}
