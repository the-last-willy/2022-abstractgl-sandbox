#version 460 core

in vec3 vertex_normal;

out vec3 fragment_color;

void main() {
    vec3 normal = normalize(vertex_normal);

    float lambertian = max(dot(normal, vec3(0., 0., -1.)), 0.);

    fragment_color = vec3(lambertian / 2. + .25);
}
