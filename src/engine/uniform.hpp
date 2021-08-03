#pragma once

#include <agl/all.hpp>

struct AnyUniform {
    virtual ~AnyUniform() = default;

    virtual void set(agl::Program, agl::UniformIndex<GLint>) const = 0;
};

template<typename Value>
struct Uniform : AnyUniform {
    Value value;

    constexpr
    Uniform(Value&& v) noexcept
        : value(std::move(v))
    {}

    void set(agl::Program p, agl::UniformIndex<GLint> ui) const override {
        agl::uniform(p, ui, value);
    }
};
