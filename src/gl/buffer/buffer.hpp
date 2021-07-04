#pragma once

#include "gl/object/object.hpp"

namespace tlw {
namespace gl {

struct BufferTraits {
    static GLuint create() {
        GLuint name;
        glCreateBuffers(1, &name);
        return name;
    }

    static void delete_(GLuint name) {
        glDeleteBuffers(1, &name);
    }

    static GLboolean is(GLuint name) {
        return glIsBuffer(name);
    }
};

using Buffer = Object<BufferTraits>;

}}
