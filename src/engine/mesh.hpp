#pragma once

#include "primitive.hpp"

#include <memory>

namespace eng {

struct Mesh {
    std::deque<std::shared_ptr<Primitive>> primitives = {};
};

}
