#pragma once

#include "gl/object/object.hpp"

namespace tlw {
namespace gl {

struct VertexArrayTraits {
    static GLuint create() {
        GLuint name;
        glCreateVertexArrays(1, &name);
        return name;
    }

    static void delete_(GLuint name) {
        glDeleteVertexArrays(1, &name);
    }

    static GLboolean is(GLuint name) {
        return glIsVertexArray(name);
    }
};

using VertexArray = Object<VertexArrayTraits>;

}}
