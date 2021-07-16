#pragma once

#include "qualifier.hpp"

namespace agl {

struct Shader : Qualifier<GLuint> {
    using Qualifier<GLuint>::Qualifier;
};

}
