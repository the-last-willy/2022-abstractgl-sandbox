#pragma once

#include "accessor.hpp"
#include "node.hpp"
#include "transform.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace eng {

struct Joint {
    std::vector<std::shared_ptr<Joint>> children = {};
    std::shared_ptr<Node> node = nullptr;
    std::shared_ptr<Joint> parent = nullptr;

    Transform global_transform = {};
};

inline
void compute_transforms(
    Joint& joint,
    const eng::Transform& parent_transform = {})
{
    joint.global_transform = parent_transform * joint.node->transform;
    for(auto& c : joint.children) {
        compute_transforms(*c, joint.global_transform);
    }
}

struct Skin {
    std::optional<Accessor> inverse_bind_matrices = std::nullopt;

    std::vector<std::shared_ptr<Node>> joints = {};

    // Optional.
    std::shared_ptr<Node> skeleton = std::nullopt;
};

}
