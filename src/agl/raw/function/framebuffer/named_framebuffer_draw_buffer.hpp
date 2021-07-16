#pragma once

#include "agl/raw/qualifier/framebuffer_buffer.hpp"
#include "agl/raw/qualifier/framebuffer.hpp"

#include <span>

namespace agl {

inline
void named_framebuffer_draw_buffer(Framebuffer f, FramebufferBuffer fb) {
    glNamedFramebufferDrawBuffer(
        static_cast<GLuint>(f),
        static_cast<GLenum>(fb));
}

inline
void draw_buffer(Framebuffer f, FramebufferBuffer fb) {
    named_framebuffer_draw_buffer(f, fb);
}

}
