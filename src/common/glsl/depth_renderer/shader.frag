#version 450 core

in vec3 v_world_normal;
in vec3 v_world_position;

out vec4 f_color;

vec3 flat_normal() {
    return normalize(cross(dFdx(v_world_position), dFdy(v_world_position)));
}

void main() {
    f_color = vec4(flat_normal() * .5 + .5, 1.);
}