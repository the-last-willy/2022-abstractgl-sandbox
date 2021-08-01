#version 460 core

uniform vec3 color_rgb = vec3(1.f, 1.f, 1.f);

in vec3 vertex_normal3;
in vec3 vertex_texcoords3;

out vec3 color;

void main() {
    vec3 light_dir = vec3(0.f, 0.f, 1.f);

    float lambertian = max(dot(vertex_normal3, -light_dir), 0.f);
    vec3 diffuse = (lambertian / 2.f + 0.5f) * color_rgb;

    color = vertex_texcoords3 / 2.f + 0.5f;
}
