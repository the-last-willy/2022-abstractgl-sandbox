#version 460 core

uniform mat4 mvp;

layout(location = 0) in vec3 position3;

void main() {
    gl_Position = mvp * vec4(position3, 1.f);
}
