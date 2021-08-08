#pragma once

#include "program.hpp"

#include <agl/all.hpp>

#include <functional>
#include <iostream>
#include <tuple>

namespace eng {

struct Material {
    eng::Program program = {};

    std::vector<std::tuple<std::string, agl::Texture>> textures = {};

    std::function<void()> on_enter = [](){};
};

inline
void bind_textures(const Material& m) {
    auto unit = 0;
    for(auto [uniform, texture] : m.textures) {
        auto ul = uniform_location(m.program.program, uniform.c_str());
        if(ul) {
            bind(agl::TextureUnit(unit), texture);
            agl::uniform(m.program.program, *ul, unit);
            unit += 1;
        } else {
            std::cout << uniform << std::endl;
        }
    }
}

inline
void bind(const Material& m) {
    bind(m.program);
    bind_textures(m);
    m.on_enter();
}

inline
void unbind(const Material& m) {
    unbind(m.program);
    // Should texture units be unbound ?
}

}
