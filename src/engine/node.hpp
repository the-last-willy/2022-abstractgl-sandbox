#pragma once

#include "camera.hpp"
#include "mesh.hpp"

#include <agl/all.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace eng {

struct Node {
    std::vector<std::shared_ptr<Node>> children = {};

    agl::Mat4 transform = agl::mat4(agl::identity);
    
    std::optional<std::shared_ptr<Camera>> camera = std::nullopt;
    std::optional<std::shared_ptr<Mesh>> mesh = std::nullopt;
};

template<typename F>
void traverse(Node& n, F f, agl::Mat4 parent_transform = mat4(agl::identity)) {
    auto transform = parent_transform * n.transform;
    if(n.mesh) {
        f(**n.mesh, transform);
    }
    for(auto& c : n.children) {
        traverse(*c, f, transform);
    }
}

template<typename F>
void traverse_cameras(Node& n, F f, agl::Mat4 parent_transform = mat4(agl::identity)) {
    auto transform = parent_transform * n.transform;
    if(n.camera) {
        f(**n.camera, transform);
    }
    for(auto& c : n.children) {
        traverse_cameras(*c, f, transform);
    }
}

}
