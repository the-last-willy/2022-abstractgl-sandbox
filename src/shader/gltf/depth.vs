#version 460 core

uniform mat4 mvp;

layout(location = 0) in vec3 POSITION;

out float vertex_ndc_z;

void main() {
    vec4 position = mvp * vec4(POSITION, 1.);

    vertex_ndc_z = position.z / position.w;

    gl_Position = position;
}
