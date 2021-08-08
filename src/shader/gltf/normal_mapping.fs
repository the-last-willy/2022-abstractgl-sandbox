#version 460 core

uniform sampler2D normalTexture;

in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texcoord;

out vec3 fragment_color;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 tangent = normalize(vertex_tangent);

    vec3 bitangent = cross(normal, tangent);

    mat3 btn = mat3(bitangent, tangent, normal);
    vec3 normal_mapping = texture(normalTexture, vertex_texcoord).rgb * 2. - 1.;
    vec3 mapped_normal = normal_mapping * btn;

    float lambertian = max(dot(mapped_normal, vec3(0., 0., -1.)), 0.);

    fragment_color = vec3(lambertian) / 2. + .5;
}
