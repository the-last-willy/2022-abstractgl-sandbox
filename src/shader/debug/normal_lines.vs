#version 460 core

uniform float length = 10.f;
uniform mat4 mvp;

layout(location = 0) in vec2 coords;
layout(location = 1) in float height;
layout(location = 2) in vec3 normal;

out VS_OUT {
    vec3 vertex_normal;
} vs_out;

void main() {
    vec3 position = vec3(coords.x, height, coords.y);

    // mat4 normal_matrix = transpose(inverse(mv));
    vs_out.vertex_normal = length * normalize((mvp * vec4(normal, 0.f)).xyz);
    
    gl_Position = mvp * vec4(position, 1.f);
}
