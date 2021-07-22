#pragma once

#include "agl/all.hpp"

#include <iostream>
#include <vector>

namespace turtle {

struct Joint {
    agl::Vec3 offset = agl::vec3(agl::null);

    std::vector<Joint> children = {};
};

inline
void print(const Joint& j, int indent = 0) {
    for(int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    for(int i = 0; i < 3; ++i) {
        std::cout << j.offset[i] << " ";
    }
    std::cout << std::endl;
    for(auto& c : j.children) {
        print(c, indent + 1);
    }
}

}
