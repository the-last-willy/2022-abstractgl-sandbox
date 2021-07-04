#pragma once

#include "gl/object/object.hpp"

namespace tlw {
namespace gl {

template<typename Subtraits>
struct TextureTraits : Subtraits {
    static void delete_(GLuint name) {
        glDeleteTextures(1, &name);
    }

    static GLboolean is(GLuint name) {
        return glIsTexture(name);
    }
};

template<typename Any>
using Texture = Object<TextureTraits<Any>>;

}}
