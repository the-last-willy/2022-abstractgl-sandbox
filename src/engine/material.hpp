#pragma once

#include "program.hpp"
#include "uniform.hpp"

#include <agl/all.hpp>

#include <functional>
#include <map>
#include <memory>
#include <tuple>

namespace eng {

struct Material {
    eng::Program program = {};

    std::map<std::string, agl::Texture> textures = {};

    std::map<std::string, AnyUniform*> uniforms = {};

    std::function<void()> on_bind = [](){};
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
            // std::cout << uniform << std::endl;
        }
    }
    m.on_bind();
}

inline
void bind(const Material& m) {
    bind(m.program);
    bind_textures(m);
}

inline
void unbind(const Material& m) {
    unbind(m.program);
    // Should texture units be unbound ?
}

}
