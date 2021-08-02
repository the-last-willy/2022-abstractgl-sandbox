#version 460 core

// uniform vec3 color_rgb = vec3(1.f, 1.f, 1.f);

in vec3 vertex_color_rgb;
in vec3 vertex_normal3;
in vec3 vertex_texcoords3;

out vec3 color;

void main() {
    vec3 normal3 = normalize(vertex_normal3);
    vec3 texcoords3 = normalize(vertex_texcoords3);

    vec3 light_dir = vec3(0.f, 0.f, 1.f);

    float lambertian = abs(dot(normal3, -light_dir));
    vec3 diffuse = (lambertian / 2.f + 0.5f) * vec3(1.f);

    color = diffuse;
    // color = mix(vec3(1.f, 0.f, 0.f), vec3(1.f), length(normal3));
}
