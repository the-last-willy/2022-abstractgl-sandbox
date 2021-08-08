#version 460 core

uniform sampler2D baseColorTexture;

in vec3 vertex_normal;
in vec2 vertex_texcoord;

out vec4 fragment_color;

void main() {
    vec3 normal = normalize(vertex_normal);

    float lambertian = max(dot(normal, vec3(0., 0., -1.)), 0.);
    vec4 diffuse = (lambertian / 2. + 0.5) * texture(baseColorTexture, vertex_texcoord).rgba;

    fragment_color = diffuse;
}
