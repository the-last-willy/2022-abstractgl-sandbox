#version 460

uniform sampler2D tex;

in vec3 vertex_normal;
in vec2 vertex_uv;

out vec3 fragment_color;

void main() {
    vec3 light_dir = normalize(vec3(-1, -1, 0.f));
    fragment_color = texture(tex, vertex_uv).xyz;
    fragment_color *= 0.5f + max(dot(vertex_normal, -light_dir), 0.f) / 2.f;
}
