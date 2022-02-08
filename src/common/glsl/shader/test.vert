#version 450 core

uniform mat4 object_to_clip;

in vec3 a_normal;
in vec3 a_position;

out vec3 v_color;

void main() {
    v_color = a_position * .5 + .5;

    gl_Position = object_to_clip * vec4(a_position, 1.);
}
