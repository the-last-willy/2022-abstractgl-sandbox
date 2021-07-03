#version 460

uniform sampler2D tex;

in vec2 vertex_uv;

out vec3 fragment_color;

void main() {
    float u = vertex_uv[0];
    float v = vertex_uv[1];
    fragment_color = texture(tex, vec2(u, 1.f - v)).xyz;
}
