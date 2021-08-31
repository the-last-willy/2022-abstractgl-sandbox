#version 460 core

uniform mat4 mvp;

// LOCATION TIED TO G_BUFFER SHADER.
layout(location = 2) in vec3 POSITION;

out vec3 vertex_pos;

void main() {
    vec4 position = mvp * vec4(POSITION, 1.);

    vertex_pos = position.xyz;

    gl_Position = position;
}
