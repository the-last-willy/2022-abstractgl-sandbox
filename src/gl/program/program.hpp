#pragma once

#include "gl/object/object.hpp"

namespace tlw {
namespace gl {

struct ProgramTraits {
    static GLuint create() {
        return glCreateProgram();
    }

    static void delete_(GLuint name) {
        glDeleteProgram(name);
    }

    static GLboolean is(GLuint name) {
        return glIsProgram(name);
    }
};

using Program = Object<ProgramTraits>;

}}
