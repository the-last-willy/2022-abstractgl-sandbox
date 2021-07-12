#version 460 core

uniform mat4 mvp;

layout(location = 0) in vec2 coords;
layout(location = 1) in float height;

void main() {
    gl_Position = mvp * vec4(coords.x, height, coords.y, 1.f);
}
