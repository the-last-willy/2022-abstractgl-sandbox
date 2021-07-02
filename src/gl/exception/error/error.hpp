#pragma once

#include <glad/glad.h>

namespace tlw {
namespace gl {

class Error {
public:
    Error(GLenum category) noexcept
        : category_{category}
    {}

    virtual ~Error() noexcept = default;

    GLenum category() const noexcept {
        return category_;
    }

private:
    GLenum category_ = GL_NO_ERROR;
};

}}
