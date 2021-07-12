#pragma once

#include "location.hpp"

#include "gl/program/program.hpp"
#include "matrix/matrix.hpp"
#include "vector/vector.hpp"

namespace tlw {
namespace gl {

bool try_uniform(const Program& p, const char* name, float f) {
    auto l = optional_uniform_location(p, name);
    if(l) {
        glUniform1f(*l, f);
        return true;
    } else {
        return false;
    }
}

bool try_uniform(const Program& p, const char* name, const Vec3& v) {
    auto l = optional_uniform_location(p, name);
    if(l) {
        glUniform3fv(*l, 1, v.elements.data());
        return true;
    } else {
        return false;
    }
}

bool try_uniform(const Program& p, const char* name, const Mat4& m) {
    auto l = optional_uniform_location(p, name);
    if(l) {
        glUniformMatrix4fv(*l, 1, GL_FALSE, m.elements.data());
        return true;
    } else {
        return false;
    }
}

}}
