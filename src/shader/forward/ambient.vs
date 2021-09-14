#version 450 core

uniform mat4 mvp_transform;

in vec3 COLOR;
in vec3 POSITION;

out vec3 vertex_color;

void main() {
    vertex_color = COLOR;

    gl_Position = mvp_transform * vec4(POSITION, 1.);
}
