#pragma once

#include <glad/glad.h>

namespace agl {

enum class BufferTarget : GLenum {
    array = GL_ARRAY_BUFFER,
    element_array = GL_ELEMENT_ARRAY_BUFFER,
    pixel_pack = GL_PIXEL_PACK_BUFFER,
    pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
    texture = GL_TEXTURE_BUFFER,
    uniform = GL_UNIFORM_BUFFER,
    // TODO: Add the remaining ones.
};

}
