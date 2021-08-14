#version 460 core

uniform mat4 mvp;

// LOCATION BADNESS, TIED TO G_BUFFER SHADER.
layout(location = 1) in vec3 POSITION;

void main() {
    gl_Position = mvp * vec4(POSITION, 1.);
}
