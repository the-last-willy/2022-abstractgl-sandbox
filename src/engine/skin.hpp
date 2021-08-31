#pragma once

#include "accessor.hpp"
#include "node.hpp"
#include "transform.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace eng {

struct Skin {
    Accessor inverse_bind_matrices = {};

    // Required.
    std::vector<std::shared_ptr<Node>> joints = {};

    // Optional.
    std::shared_ptr<Node> skeleton = nullptr;
};

}
