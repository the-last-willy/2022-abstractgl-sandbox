#pragma once

#include "qualifier.hpp"

namespace agl {

struct Framebuffer : Qualifier<GLuint> {
    using Qualifier<GLuint>::Qualifier;
};

}
