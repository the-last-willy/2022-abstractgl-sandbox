#version 460 core

in vec3 vertex_normal3;

out vec3 fragment_color;

void main() {
    vec3 light_dir = vec3(0.f, 0.f, 1.f);

    float lambertian = abs(dot(vertex_normal3, light_dir));
    vec3 diffuse = (lambertian / 2.f + 0.5f) * vec3(0.f, 153.f, 0.f) / 255.f;

    fragment_color = diffuse;

    fragment_color = vertex_normal3 / 2.f + 0.5f;
}
