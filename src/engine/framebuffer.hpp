#pragma once

#include "material.hpp"
#include "texture.hpp"

#include <agl/all.hpp>

#include <map>
#include <memory>
#include <functional>

namespace eng {

struct Framebuffer {
    agl::Framebuffer opengl = agl::none;

    std::map<std::string, std::shared_ptr<Texture>> color_attachments = {};

    std::function<void(const agl::Framebuffer&)> on_clear
    = [](const agl::Framebuffer&) {};
};

inline
void bind(const Framebuffer& f) {
    agl::bind(f.opengl);
}

void unbind(const Framebuffer&) {
    agl::bind(agl::default_framebuffer);
}

inline
void clear(const Framebuffer& f) {
    f.on_clear(f.opengl);
}

}
