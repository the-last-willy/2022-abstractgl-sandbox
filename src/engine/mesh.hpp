#pragma once

#include "primitive.hpp"

#include <memory>

namespace eng {

struct Mesh {
    std::shared_ptr<eng::Material> material = {};
    std::deque<std::shared_ptr<Primitive>> primitives = {};
};

}
