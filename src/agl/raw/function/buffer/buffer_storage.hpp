#pragma once

#include "agl/raw/qualifier/buffer.hpp"

namespace agl {

[[deprecated("Use 'named_buffer_storage' instead.")]]
void buffer_storage(GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);

}
