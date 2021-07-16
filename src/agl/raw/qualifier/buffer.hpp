#pragma once

#include "qualifier.hpp"

namespace agl {

struct Buffer : Qualifier<GLuint> {
    using Qualifier<GLuint>::Qualifier;
};

}
