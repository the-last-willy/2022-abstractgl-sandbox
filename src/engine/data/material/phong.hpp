#pragma once

#include "dependencies.hpp"

inline
auto make_monochrome_phong_material() {
    auto material = Material();
    { // Capabilities.
        material.capabilities.insert(
            end(material.capabilities),
            {
                agl::Capability::cull_face,
                agl::Capability::depth_test,
            });
    }
    { // Parameters.
        material.parameters.push_back([]() {
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        });
    }
    { // Program.
        auto& p = material.program;
        auto shaders = std::map<agl::ShaderType, std::string>{
            {
                agl::ShaderType::vertex,
                file(root + "src/shader/rock/phong_monochrome.vs")
            },
            {
                agl::ShaderType::fragment,
                file(root + "src/shader/rock/phong_monochrome.fs")
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
