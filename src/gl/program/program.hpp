#pragma once

#include "gl/object/object.hpp"

#include <map>
#include <string>

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

inline
auto program(const std::map<GLenum, const char*>& types_sources) {
    auto p = Program();
    for(const auto& [type, source] : types_sources) {
        auto s = glCreateShader(type);
        glShaderSource(s, 1, &source, NULL);
        glCompileShader(s);
        glAttachShader(p, s);
        glDeleteShader(s);
    }
    glLinkProgram(p);
    return p;
}

}}
