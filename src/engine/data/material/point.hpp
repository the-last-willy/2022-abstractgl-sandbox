#pragma once

#include "dependencies.hpp"

inline
auto make_point_material() {
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
            glCullFace(GL_BACK);
            glPointSize(10.f);
            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        });
    }
    { // Program.
        auto& p = material.program;
        auto shaders = std::map<agl::ShaderType, std::string>{
            {
                agl::vertex_shader_tag,
                file(root + "src/shader/rock/point.vs")
            },
            {
                agl::fragment_shader_tag,
                file(root + "src/shader/rock/point.fs")
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
