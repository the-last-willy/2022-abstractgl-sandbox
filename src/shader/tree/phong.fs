#version 460 core

in vec3 vertex_normal3;

out vec3 color;

void main() {
    vec3 light_dir = vec3(0.f, 0.f, 1.f);

    float lambertian = max(dot(vertex_normal3, -light_dir), 0.f);
    vec3 diffuse = (lambertian / 2.f + 0.5f) * vec3(135.f, 62.f, 35.f) / 255.f;

    color = diffuse;
}
