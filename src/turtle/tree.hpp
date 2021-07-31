#pragma once

#include <agl/all.hpp>

#include <functional>
#include <iostream>
#include <vector>

namespace turtle {

struct Tree {
    unsigned model_id = 0;

    std::vector<agl::Vec3> boundary = {};

    // agl::Mat4 relative_transform = agl::mat4(agl::identity);
    agl::Mat4 world_transform = agl::mat4(agl::identity);

    std::vector<Tree> children = {};
};

template<typename Type>
concept TreeTraverser = requires(Type t) {
    t.accept(std::declval<Tree>());
    t.decrement_depth();
    t.increment_depth();
};

template<TreeTraverser TT>
void traverse(Tree& t, TT& tt) {
    auto counters = std::vector<std::size_t>{2};
    auto stack = std::vector<Tree*>{&t};
    while(!empty(stack)) {
        auto& current_tree = *stack.back();
        stack.pop_back();
        tt.accept(current_tree);
        counters.back() -= 1;
        if(empty(current_tree.children)) {
            while(counters.back() == 0) {
                counters.pop_back();
                tt.decrement_depth();
            }
        } else {
            counters.push_back(size(current_tree.children));
            tt.increment_depth();
            for(auto& c : current_tree.children) {
                stack.push_back(&c);
            }
        }
    }
}

struct TreePrinter {
    std::size_t depth = 0;

    void accept(const Tree& t) {
        for(std::size_t i = 0; i < depth; ++i) {
            std::cout << " "; 
        }
        std::cout << t.model_id << std::endl;
    }

    void decrement_depth() {
        std::cout << std::string(--depth, ' ') << "-" << std::endl;
    }

    void increment_depth() {
        std::cout << std::string(depth++, ' ') << "+" << std::endl;
    }
};

struct TreeLeaves {
    std::vector<const Tree*> leaves = {};

    void accept(const Tree& t) {
        if(!empty(t.boundary)) {
            leaves.push_back(&t);
        }
    } 

    void decrement_depth() {

    }

    void increment_depth() {

    }
};

struct TreeTransforms {
    std::vector<agl::Mat4> transforms = {};

    void accept(const Tree& t) {
        if(empty(t.boundary)) {
            transforms.push_back(t.world_transform);
        }
    } 

    void decrement_depth() {

    }

    void increment_depth() {

    }
};

}
