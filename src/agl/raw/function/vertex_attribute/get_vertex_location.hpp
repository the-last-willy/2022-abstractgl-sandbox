#pragma once

#include "agl/raw/qualifier/attribute_index.hpp"
#include "agl/raw/qualifier/cstring.hpp"
#include "agl/raw/qualifier/program.hpp"

namespace agl {

inline
AttributeIndex<GLint> get_attrib_location(Program p, cstring name) {
    return AttributeIndex<GLint>(glGetAttribLocation(
            static_cast<GLuint>(p),
            name));
}

inline
AttributeIndex<GLint> attribute_location(Program p, cstring name) {
    return get_attrib_location(p, name);
}

}
