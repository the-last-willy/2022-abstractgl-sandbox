#pragma once

#include <agl/all.hpp>

#include <optional>

namespace eng {

struct Texture {
    std::optional<agl::Sampler> sampler = std::nullopt;
    agl::Texture texture = {};
};

}
