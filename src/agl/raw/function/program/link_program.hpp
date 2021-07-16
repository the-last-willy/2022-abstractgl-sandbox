#pragma once

#include "agl/raw/qualifier/program.hpp"

namespace agl {

inline
void link_program(Program p) {
    glLinkProgram(static_cast<GLuint>(p));
}

inline
void link(Program p) {
    link_program(p);
}

}
