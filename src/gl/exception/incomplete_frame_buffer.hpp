#pragma once

namespace tlw {
namespace gl {

#include <glad/glad.h>

class IncompleteFrameBuffer {
public:
    IncompleteFrameBuffer(GLenum category) noexcept
        : category_{category}
    {}

    virtual ~IncompleteFrameBuffer() noexcept = default;

    GLenum category() const noexcept {
        return category_;
    }

private:
    GLenum category_ = GL_NO_ERROR;
};

}}
