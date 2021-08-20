#pragma once

#include "program.hpp"
#include "texture.hpp"
#include "uniform.hpp"

#include <agl/all.hpp>

#include <functional>
#include <map>
#include <memory>
#include <tuple>

namespace eng {

struct Material {
    eng::Program program = {};

    std::map<std::string, std::shared_ptr<eng::Texture>> textures = {};

    std::map<std::string, AnyUniform*> uniforms = {};

    std::function<void()> on_bind = [](){};
};

inline
void bind_textures(const Material& m) {
    auto unit = 0;
    for(auto [name, texture_ptr] : m.textures) {
        auto& texture = *texture_ptr;
        auto ul = uniform_location(m.program.program, name.c_str());
        if(ul) {
            if(texture.sampler) {
                bind(agl::TextureUnit(unit), *texture.sampler);
            }
            bind(agl::TextureUnit(unit), texture.texture);
            agl::uniform(m.program.program, *ul, unit);
            unit += 1;
        }
    }
}

inline
void bind_uniforms(const Material& m) {
    for(auto& [name, value] : m.uniforms) {
        auto ul = uniform_location(m.program.program, name.c_str());
        if(ul) {
            value->set(m.program.program, *ul);
        }
    }
    m.on_bind();
}

inline
void bind(const Material& m) {
    bind(m.program);
    bind_textures(m);
    bind_uniforms(m);
    m.on_bind();
}

inline
void unbind(const Material& m) {
    unbind(m.program);
    // Should texture units be unbound ?
}

}
