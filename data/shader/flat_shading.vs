#version 460

uniform mat4 m;
uniform mat4 mvp;

in vec3 position;

out vec3 vertex_position;

void main() {
    vertex_position = (m * vec4(position, 1.f)).xyz;

    gl_Position = mvp * vec4(position, 1.f);
}
