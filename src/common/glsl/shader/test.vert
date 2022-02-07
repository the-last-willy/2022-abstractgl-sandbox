#version 450 core

in vec3 a_color;
in vec3 a_position;

out vec3 v_color;

void main() {
    v_color = a_color;

    gl_Position = vec4(a_position, 1.);
}
