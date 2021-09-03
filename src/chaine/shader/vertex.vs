#version 450

uniform mat4 mvp;

layout(location = 0) in vec3 POSITION;

void main() {
    gl_Position = mvp * vec4(POSITION, 1.);
}
