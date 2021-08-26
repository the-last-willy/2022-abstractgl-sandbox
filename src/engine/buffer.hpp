#pragma once

#include <agl/all.hpp>

#include <vector>

namespace eng {

struct Buffer {
    std::vector<std::byte> data = {};
    agl::Buffer opengl = {};
};

}
