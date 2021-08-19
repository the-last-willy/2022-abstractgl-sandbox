#pragma once

#include "node.hpp"

#include <memory>
#include <vector>

namespace eng {

struct Scene {
    std::vector<std::shared_ptr<Node>> nodes = {};
};

}
