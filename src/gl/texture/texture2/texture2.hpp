#pragma once

#include "gl/texture/texture.hpp"

namespace tlw {
namespace gl {

struct Texture2Subtraits {
    static GLuint create() {
        GLuint name;
        glCreateTextures(GL_TEXTURE_2D, 1, &name);
        return name;
    }
};

using Texture2 = Texture<Texture2Subtraits>;

}}
