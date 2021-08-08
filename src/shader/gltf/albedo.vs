#version 460 core

uniform mat4 mvp;

layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec2 TEXCOORD_0;

out vec2 vertex_texcoord;

void main() {
    vertex_texcoord = TEXCOORD_0;

    gl_Position = mvp * vec4(POSITION, 1.);
}
