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

enum class AlphaMode {
    blend,
    mask,
    opaque  
};

struct Material {
    AlphaMode alpha_mode = AlphaMode::opaque;

    // SHOULD BE REMOVED.
    Program program = {};

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

////////////////////////////////////////////////////////////////////////////////
// No program dependency inside material.

inline
void bind_textures(const Material& m, const Program& p) {
    auto unit = 0;
    for(auto [name, texture_ptr] : m.textures) {
        auto& texture = *texture_ptr;
        auto ul = uniform_location(p.program, name.c_str());
        if(ul) {
            if(texture.sampler) {
                bind(agl::TextureUnit(unit), *texture.sampler);
            }
            bind(agl::TextureUnit(unit), texture.texture);
            agl::uniform(p.program, *ul, unit);
            unit += 1;
        }
    }
}

inline
void bind_uniforms(const Material& m, const Program& p) {
    for(auto& [name, value] : m.uniforms) {
        auto ul = uniform_location(p.program, name.c_str());
        if(ul) {
            value->set(p.program, *ul);
        }
    }
    m.on_bind();
}

inline
void bind(const Material& m, const Program& p) {
    bind_textures(m, p);
    bind_uniforms(m, p);
    m.on_bind();
}

}
