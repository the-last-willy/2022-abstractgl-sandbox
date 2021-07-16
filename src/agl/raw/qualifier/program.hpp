#pragma once

#include "qualifier.hpp"

namespace agl {

struct Program : Qualifier<GLuint> {
    using Qualifier<GLuint>::Qualifier;
};

}
