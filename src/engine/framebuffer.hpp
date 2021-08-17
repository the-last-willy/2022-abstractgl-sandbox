#pragma once

#include <agl/all.hpp>

#include <functional>

namespace eng {

struct Framebuffer {
    agl::Framebuffer opengl = agl::none;

    static void no_clear(const agl::Framebuffer&) {}

    std::function<void(const agl::Framebuffer&)> on_clear = &no_clear;
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
