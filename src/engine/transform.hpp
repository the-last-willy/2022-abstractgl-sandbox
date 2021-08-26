#pragma once

#include <agl/all.hpp>

#include <variant>

namespace eng {

struct TransformTRS {
    agl::Vec3 translation = agl::vec3(0.f);
    agl::Quaternion<float> rotation = {};
    agl::Vec3 scaling = agl::vec3(1.f);
};

inline
auto mat4(const TransformTRS& trs) {
    return agl::translation(trs.translation)
    * agl::mat4(trs.rotation)
    * agl::scaling3(trs.scaling[0], trs.scaling[1], trs.scaling[2]);
}

struct Transform {
    std::variant<TransformTRS, agl::Mat4> representation = {};

    Transform() noexcept = default;

    Transform(agl::Mat4&& m)
        : representation(std::move(m))
    {}

    Transform(TransformTRS&& trs)
        : representation(std::move(trs))
    {}
};

inline
auto mat4(const Transform& t) {
    if(auto ptr = std::get_if<agl::Mat4>(&t.representation)) {
        return *ptr;
    }
    if(auto ptr = std::get_if<TransformTRS>(&t.representation)) {
        return mat4(*ptr);
    }
    throw std::runtime_error("Unhandled variant alternative.");
}

inline
auto operator*(const Transform& lt, const Transform& rt) {
    struct {
        Transform operator()(const agl::Mat4& l, const agl::Mat4& r) const {
            return l * r;
        }
        Transform operator()(const agl::Mat4& l, const TransformTRS& r) const {
            return l * mat4(r);
        }
        Transform operator()(const TransformTRS& l, const agl::Mat4& r) const {
            return mat4(l) * r;
        }
        Transform operator()(const TransformTRS& l, const TransformTRS& r) const {
            return mat4(l) * mat4(r);
        }
    } visitor;
    return std::visit(visitor, lt.representation, rt.representation);
}

}
