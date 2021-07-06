#pragma once

#include "node.hpp"

#include "matrix/all.hpp"

#include <vector>

namespace tlw {
namespace scene {

struct Renderer {

    void visit(const Group<Renderer>& g) {
        for(auto& c : g.children) {
            c->accept(*this);
        }
    }

    void visit(const Instance<Renderer>& i) {
        auto& mat = i.material;
        auto& program = *(this->program);
        
        gl::try_uniform(program, "material.ambient",
            Vec3{mat.ambient[0], mat.ambient[1], mat.ambient[2]});
        gl::try_uniform(program, "material.diffuse",
            Vec3{mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]});
        gl::try_uniform(program, "material.shininess",
            mat.shininess);
        gl::try_uniform(program, "material.specular",
            Vec3{mat.specular[0], mat.specular[1], mat.specular[2]});
        gl::try_uniform(program, "m", models.back());
        gl::try_uniform(program, "mvp", projection * view * models.back());
        gl::bind(i.vertex_array);
        gl::draw_arrays(GL_TRIANGLES, 0, i.vertex_count);
    }

    void visit(const Transformation<Renderer>& t) {
        models.push_back(models.back() * t.mat);
        t.child->accept(*this);
        models.pop_back();
    }

    gl::Program* program = nullptr;

    Mat4 projection = mat4_identity();
    Mat4 view = mat4_identity();
    std::vector<Mat4> models = {mat4_identity()};
};

}}
