#version 460 core

uniform vec3 light_pos;

uniform float far;

in vec3 vertex_pos;

void main() {
    // 'vertex_pos.z' is in [-near, far]
    gl_FragDepth = length(vertex_pos - light_pos) / far;
}
