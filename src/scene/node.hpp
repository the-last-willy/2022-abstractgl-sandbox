#pragma once

#include "scene/exception/all.hpp"

#include "gl/all.hpp"
#include "matrix/all.hpp"

#include "tiny_obj_loader.h"

#include <memory>
#include <vector>

namespace tlw {
namespace scene {

template<typename Visitor>
struct Node {
    virtual ~Node() = default;

    virtual void accept(Visitor& v) const = 0;
};

template<typename Visitor>
struct Group : Node<Visitor> {
    void accept(Visitor& v) const override {
        v.visit(*this);
    }

    std::vector<std::unique_ptr<Node<Visitor>>> children = {}; 
};

template<typename Visitor>
struct Instance : Node<Visitor> {
    void accept(Visitor& v) const override {
        v.visit(*this);
    }

    gl::Buffer buffer = {};
    tinyobj::material_t material = {};
    gl::VertexArray vertex_array = {};
    GLsizei vertex_count = 0;
};

template<typename Visitor>
struct Transformation : Node<Visitor> {
    Transformation(Mat4 m, std::shared_ptr<Node<Visitor>> child)
        : mat(m)
        , child(std::move(child))
    {
        if(!this->child) {
            throw InvariantViolation();
        }
    }

    void accept(Visitor& v) const override {
        v.visit(*this);
    }

    Mat4 mat = mat4_identity();
    std::shared_ptr<Node<Visitor>> child = nullptr;
};

}}

