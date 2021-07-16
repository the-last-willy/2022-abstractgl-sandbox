#pragma once

#include "agl/raw/qualifier/program.hpp"

namespace agl {

inline
Program create_program() {
    return Program(glCreateProgram());
}

inline
Program program() {
    return create_program();
}

}
