#pragma once

#include "../mesh/id.hpp"

#include "common/dependency/glm.hpp"

#include <memory>
#include <vector>

struct SceneGraph {
    glm::mat4 transform = glm::mat4(1.f);

    std::vector<std::unique_ptr<SceneGraph>> children;

    std::vector<MeshId> meshes;
};
