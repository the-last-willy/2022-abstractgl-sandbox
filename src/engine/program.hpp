#pragma once

#include "capability.hpp"
#include "shader.hpp"

#include <agl/all.hpp>

#include <iostream>
#include <vector>
#include <map>

namespace eng {

struct Program {
    std::vector<Capability> capabilities = {};
    agl::Program program = {};
};

inline
void load(
    Program& p,
    const std::map<agl::ShaderType, std::string> shader_sources)
{
    p.program = agl::create(agl::program_tag);
    for(auto& [type, src] : shader_sources) {
        auto s = create(type);
        source(s, src);
        compile(s);
        agl::attach(p.program, s);
        delete_(s);
    }
    link(p.program);
}

inline
void load(
    Program& p,
    const ShaderCompiler& sc,
    const std::map<agl::ShaderType, std::string> shader_filepaths)
{
    p.program = agl::create(agl::program_tag);
    for(auto& [type, filepath] : shader_filepaths) {
        auto s = sc(filepath, type);
        agl::attach(p.program, s);
        delete_(s);
    }
    link(p.program);
}

inline
void bind(const Program& p) {
    for(auto c : p.capabilities) {
        enable(c);
    }
    use(p.program);
}

inline
void unbind(const Program& p) {
    for(auto c : p.capabilities) {
        disable(c);
    }
}

template<typename Type>
void uniform(const Program& p, agl::czstring name, const Type& t) {
    auto ul = uniform_location(p.program, name);
    if(ul) {
        agl::uniform(p.program, *ul, t);
    } else {
        // std::cerr << "No uniform named " << name << "." << std::endl;
    }
    
}

}
