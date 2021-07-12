#version 460 core

in vec3 vertex_normal;

out vec4 fragment_color;

void main() {
    vec3 light_direction = vec3(0.f, -1.f, 0.f);
    float lambertian = abs(dot(vertex_normal, light_direction)); // TODO: fix later
    float diffuse = lambertian;
    fragment_color = vec4(vec3(diffuse), 1.f);
}
