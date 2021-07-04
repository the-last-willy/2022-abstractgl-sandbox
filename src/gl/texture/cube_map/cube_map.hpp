#pragma once

#include "gl/texture/texture.hpp"

namespace tlw {
namespace gl {

struct CubeMapSubtraits {
    static GLuint create() {
        GLuint name;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &name);
        return name;
    }
};

using CubeMap = Texture<CubeMapSubtraits>;

}}
