#pragma once

#include "gl/object/object.hpp"

#include <stdexcept>

namespace tlw {
namespace gl {

class Program : public Object {
public:
    Program()
        : name_(glCreateProgram())
    {}

    ~Program() noexcept {
        glDeleteProgram(name_);
    }

    GLuint name() const {
        if(!glIsProgram(name_)) {
            throw std::runtime_error(
                "Program: Invalid object name.");
        }
        return name_;
    }

    operator GLuint() const {
        return name();
    }

private:
    GLuint name_ = 0;
};

}}
