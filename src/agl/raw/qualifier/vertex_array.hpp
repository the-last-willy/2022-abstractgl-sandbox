#pragma once

#include "qualifier.hpp"

namespace agl {

struct VertexArray : Qualifier<GLuint> {
    using Qualifier<GLuint>::Qualifier;
};

}
