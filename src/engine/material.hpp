#pragma once

#include <agl/all.hpp>

#include <functional>

struct Material {
    std::vector<agl::Capability> capabilities = {};
    agl::Program program = agl::program();
    std::vector<std::function<void()>> parameters = {};
};
