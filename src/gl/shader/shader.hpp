#pragma once

#include "gl/object/object.hpp"

namespace tlw {
namespace gl {

template<typename Subtraits>
struct ShaderTraits : Subtraits {
    static void delete_(GLuint name) {
        glDeleteShader(name);
    }

    static GLboolean is(GLuint name) {
        return glIsShader(name);
    }
};

template<typename Any>
using Shader = Object<ShaderTraits<Any>>;

}}
