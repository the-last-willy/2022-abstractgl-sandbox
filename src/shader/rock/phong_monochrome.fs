#version 460 core

uniform sampler2D albedo_texture;

// in vec3 vertex_color_rgb;
in vec3 vertex_normal3;
in vec2 vertex_texcoords2;

out vec3 fragment_color;

void main() {
    vec3 normal3 = normalize(vertex_normal3);

    vec3 light_dir = vec3(0., 0., 1.);

    float lambertian = max(dot(normal3, -light_dir), 0.);
    vec3 diffuse = (lambertian / 2. + 0.5) * texture(albedo_texture, vertex_texcoords2).rgb;

    fragment_color = diffuse;
}
