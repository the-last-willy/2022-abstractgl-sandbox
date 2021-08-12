#version 460 core

uniform float length = 1.f;
uniform mat4 mvp;

layout(location = 0) in vec3 NORMAL;
layout(location = 1) in vec3 POSITION;

out VS_OUT {
    vec3 vertex_normal;
} vs_out;

void main() {
    vec3 position = (mvp * POSITION).xyz;

    // mat4 normal_matrix = transpose(inverse(mv));
    vs_out.vertex_normal = length * normalize((mvp * vec4(normal, 0.f)).xyz);
    
    gl_Position = mvp * position;
}
