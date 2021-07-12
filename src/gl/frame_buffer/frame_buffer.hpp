#pragma once

#include "gl/object/object.hpp"

#include <glad/glad.h>

namespace tlw {
namespace gl {

struct FrameBufferTraits {
    static GLuint create() {
        GLuint name;
        glCreateFramebuffers(1, &name);
        return name;
    }

    static void delete_(GLuint name) {
        glDeleteFramebuffers(1, &name);
    }

    static GLboolean is(GLuint name) {
        return glIsFramebuffer(name);
    } 
};

using FrameBuffer = Object<FrameBufferTraits>;

}}
