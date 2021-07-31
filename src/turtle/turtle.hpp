#pragma once

#include "joint.hpp"
#include "tree.hpp"

#include "agl/all.hpp"
#include "pi.hpp"

#include <cctype>
#include <string_view>
#include <vector>

namespace turtle {

struct Turtle {
    float angle = agl::pi / 8.f;

    std::vector<std::reference_wrapper<Joint>> parent_stack = {};
    std::vector<agl::Mat4> transform_stack = {};

    std::vector<std::reference_wrapper<Tree>> parent_stack2 = {};

    Joint feed(std::string_view str) {
        auto j = Joint();
        parent_stack.push_back(j);
        transform_stack.push_back(agl::mat4(agl::identity));

        for(std::size_t i = 0; i < size(str); ++i) {
            auto c = str[i];

            auto& transform = transform_stack.back();
            
            if(c == '+') {
                transform = transform * agl::rotation_z(-angle);
            } else if(c == '-') {
                transform = transform * agl::rotation_z(+angle);
            } else if(c == '&') {
                transform = transform * agl::rotation_y(-angle);
            } else if(c == '^') {
                transform = transform * agl::rotation_y(+angle);
            } else if(c == '\\') {
                transform = transform * agl::rotation_x(+angle);
            } else if(c == '/') {
                transform = transform * agl::rotation_x(-angle);
            } else if(c == '|') {
                transform = transform * agl::rotation_z(-agl::pi);
            } else if(c == 'F') {
                auto child = Joint();

                child.offset = transform[0].xyz();
                parent_stack.back().get().children.push_back(std::move(child));
                parent_stack.back() = parent_stack.back().get().children.back();
                transform_stack.push_back(transform);
                transform_stack.back()[3] = agl::vec4(0.f, 0.f, 0.f, 1.f);
            } else if(c == '[') {
                parent_stack.push_back(parent_stack.back());
                transform_stack.push_back(transform);
                transform_stack.back()[3] = agl::vec4(0.f, 0.f, 0.f, 1.f);
            }
        } 
        return j;
    }

    Tree feed2(std::string_view str) {
        auto t = Tree();
        parent_stack2.push_back(t);
        transform_stack.push_back(agl::mat4(agl::identity));

        for(std::size_t i = 0; i < size(str); ++i) {
            auto c = str[i];

            auto& transform = transform_stack.back();
            
            if(c == '+') {
                transform = transform * agl::rotation_z(-angle);
            } else if(c == '-') {
                transform = transform * agl::rotation_z(+angle);
            } else if(c == '&') {
                transform = transform * agl::rotation_y(-angle);
            } else if(c == '^') {
                transform = transform * agl::rotation_y(+angle);
            } else if(c == '\\') {
                transform = transform * agl::rotation_x(+angle);
            } else if(c == '/') {
                transform = transform * agl::rotation_x(-angle);
            } else if(c == '|') {
                transform = transform * agl::rotation_z(-agl::pi);
            } else if(c == 'F') {
                transform = transform * agl::translation(1.f, 0.f, 0.f);
            
                auto child = Tree();
                child.world_transform = transform;
                parent_stack2.back().get().children.push_back(std::move(child));
                parent_stack2.back() = parent_stack2.back().get().children.back();
                transform_stack.back() = transform;
            } else if(c == 'f') {
                transform = transform * agl::translation(1.f, 0.f, 0.f);
                parent_stack2.back().get().boundary.push_back(transform[3].xyz());
            } else if(c == '[') {
                parent_stack2.push_back(parent_stack2.back());
                transform_stack.push_back(transform);
            } else if(c == ']') {
                parent_stack2.pop_back();
                transform_stack.pop_back();
            } else if(c == ']') {
                parent_stack.pop_back();
                transform_stack.pop_back();
            } else if(c == '{') {
                auto child = Tree();
                child.world_transform = transform;
                parent_stack2.back().get().children.push_back(std::move(child));
                parent_stack2.back() = parent_stack2.back().get().children.back();
            } else if(c == '}') {
                
            }
        }
        return t;
    }
};

}
 