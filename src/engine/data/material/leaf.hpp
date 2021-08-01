#pragma once

#include "dependencies.hpp"

inline
auto make_leaf_material() {
    auto material = Material();
    { // Capabilities.
        material.capabilities.insert(
            end(material.capabilities),
            {
                agl::Capability::depth_test,
            });
    }
    { // Parameters.
        material.parameters.push_back([]() {
            glDepthFunc(GL_LESS);
        });
    }
    { // Program.
        auto& p = material.program;
        auto shaders = std::map<agl::ShaderType, std::string>{
            {
                agl::ShaderType::vertex,
                file(root + "src/shader/tree/leaf.vs")
            },
            {
                agl::ShaderType::fragment,
                file(root + "src/shader/tree/leaf.fs")
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
