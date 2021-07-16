#pragma once

#include <glad/glad.h>

namespace agl {

enum class DrawType : GLenum {
    unsigned_byte = GL_UNSIGNED_BYTE,
    unsigned_int = GL_UNSIGNED_INT,
    unsigned_short = GL_UNSIGNED_SHORT,
};

}
