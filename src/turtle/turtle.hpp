#pragma once

#include "joint.hpp"

#include "agl/all.hpp"
#include "pi.hpp"

#include <cctype>
#include <string_view>
#include <vector>

namespace turtle {

struct Turtle {
    float angle = agl::pi / 2.f;

    std::string_view feed(std::string_view str, Joint& parent, agl::Mat4 transform) {
        transform[3] = agl::vec4(0.f, 0.f, 0.f, 1.f);
        //LF+RFR+FL-F-LFLFL-FRFR+
        for(std::size_t i = 0; i < size(str); ++i) {
            auto c = str[i];
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
                auto j = Joint();
                j.offset = (transform[0]).xyz();
                parent.children.push_back(std::move(j));
                return feed(str.substr(i + 1), parent.children.back(), transform);
            } else if(c == '[') {
                str = feed(str.substr(i + 1), parent, transform);
            } else if(c == ']') {
                return str.substr(i);
            }
        }
        return str;
    }

    Joint feed(std::string_view str) {
        auto j = Joint();
        feed(str, j, agl::mat4(agl::identity));
        return j;
    }
};

}
 