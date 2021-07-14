#pragma once

#include "matrix/matrix.hpp"
#include "vector/vector.hpp"

#include "gl/program/program.hpp"

namespace tlw {
namespace gl {

// Proxy pattern.
class Uniform {
public:
    Uniform(const Program& p, const char* name)
        : program(p)
        , location(glGetUniformLocation(p, name))
    {}

    Uniform(const Uniform&) = delete;

    Uniform(Uniform&&) = delete;

    Uniform& operator=(const Uniform&) = delete;

    Uniform& operator=(Uniform&&) = delete;

    Uniform& operator=(int i) {
        glProgramUniform1i(program, location, i);
        return *this;
    }

    Uniform& operator=(float f) {
        glProgramUniform1f(program, location, f);
        return *this;
    }

    Uniform& operator=(const Mat4& m) {
        glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, m.elements.data());
        return *this;
    }

    Uniform& operator=(const Vec3& v) {
        glProgramUniform3fv(program, location, 1, v.elements.data());
        return *this;
    }

    operator bool() const noexcept {
        return location != -1;
    }

private:
    const Program& program;
    GLint location;
};

}}
