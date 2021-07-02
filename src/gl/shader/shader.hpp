#pragma once

#include <glad/glad.h>

#include <stdexcept>

namespace tlw {

struct Shader {
    Shader(GLenum type)
        : id_{glCreateShader(type)}
    {
        if(id_ == 0) {
            auto error = glGetError();
            if(error == GL_INVALID_ENUM) {
                throw std::logic_error("Shader: Invalid enum.");
            } else {
                throw std::runtime_error("Shader: \"glCreateShader\" failed.");
            }
        }
    }

    ~Shader() noexcept {
        glDeleteShader(id_);
    }

    GLuint id() const {
        if(!glIsShader(id_)) {
            throw std::runtime_error("Shader: Not a valid shader id.");
        }
        return id_;
    }

    operator GLuint() const {
        return id();
    }

private:
    GLuint id_ = 0;
};

inline
auto fragment_shader() {
    return Shader(GL_FRAGMENT_SHADER);
}

inline
auto vertex_shader() {
    return Shader(GL_VERTEX_SHADER);
}

}
