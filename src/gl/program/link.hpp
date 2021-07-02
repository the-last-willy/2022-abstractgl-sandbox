#pragma once

#include "link_status.hpp"
#include "program.hpp"

#include "gl/exception/error/throw_if_error.hpp"
#include "gl/exception/linkage_failure.hpp"

namespace tlw {
namespace gl {

inline
auto try_link(const Program& p) {
    glLinkProgram(p);
    throw_if_error();
    return link_status(p);
}

inline
auto link(const Program& p) {
    auto success = try_link(p);
    if(!success) {
        throw LinkageFailure();
    }
}

}}

