#pragma once

#include "agl/raw/qualifier/framebuffer_buffer.hpp"
#include "agl/raw/qualifier/framebuffer.hpp"

#include <span>

namespace agl {

inline
void named_framebuffer_draw_buffers(Framebuffer f, GLsizei n, FramebufferBuffer* fbs) {
    glNamedFramebufferDrawBuffers(
        static_cast<GLuint>(f),
        n,
        reinterpret_cast<GLenum*>(fbs));
}

inline
void draw_buffers(Framebuffer f, std::span<FramebufferBuffer> s) {
    named_framebuffer_draw_buffers(f,
        static_cast<GLsizei>(size(s)),
        data(s));
}

}