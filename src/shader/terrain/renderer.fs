#version 460 core

uniform vec3 light_direction;
uniform sampler2D tex;

in vec3 vertex_normal;
in vec2 vertex_uv;

out vec4 fragment_color;

void main() {
    float lambertian = max(dot(vertex_normal, -light_direction), 0.f);
    lambertian = lambertian / 2.f + 0.5f;
    vec4 diffuse = lambertian * texture(tex, vertex_uv);
    fragment_color = diffuse;
    // fragment_color = vec4(vertex_uv, 0.5f, 1.f);
}
