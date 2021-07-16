#version 460 core

uniform mat4 vp;

layout(location = 0) in mat4 model;
layout(location = 4) in vec3 position;

void main() {
    model;
    gl_Position = vp * model * vec4(position, 1.f);
}
