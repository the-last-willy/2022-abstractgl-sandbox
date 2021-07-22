#version 460 core

uniform mat4 mvp;

layout(location = 0) in vec3 normal3;
layout(location = 1) in vec3 position3;

out vec3 vertex_normal3;

void main() {
    vertex_normal3 = normal3;

    gl_Position = mvp * vec4(position3, 1.f);
}
