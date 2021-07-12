#pragma once

#include "frame_buffer.hpp"

#include "gl/exception/incomplete_frame_buffer.hpp"

#include <glad/glad.h>

namespace tlw {
namespace gl {

inline
void throw_if_incomplete(const FrameBuffer& fb) {
    auto e = glCheckNamedFramebufferStatus(fb, GL_FRAMEBUFFER);
    if(e != GL_FRAMEBUFFER_COMPLETE) {
        throw gl::IncompleteFrameBuffer(e);
    }
}

}}
