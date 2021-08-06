#pragma once

#include "dependencies.hpp"

inline
auto make_baking_rock_texture_material() {
    auto material = Material();
    { // Program.
        auto& p = material.program;
        auto shaders = std::map<agl::ShaderType, std::string>{
            {
                agl::ShaderType::vertex,
                file(root + "src/shader/rock/bake_texture.vs")
            },
            {
                agl::ShaderType::fragment,
                file(root + "src/shader/rock/bake_texture.fs")
            },
        };
        for(auto& [type, src] : shaders) {
            auto s = agl::shader(type);
            agl::source(s, src);
            agl::compile(s);
            agl::attach(p, s);
            agl::delete_(s);
        }
        agl::link(p);
    }
    return material;
}
