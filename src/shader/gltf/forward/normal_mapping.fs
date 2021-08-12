#version 460 core

uniform sampler2D normalTexture;

in vec3 vertex_normal;
in vec4 vertex_tangent;
in vec2 vertex_texcoord;

out vec3 fragment_color;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 tangent = normalize(vertex_tangent.xyz);

    vec3 bitangent = -cross(normal, tangent) * vertex_tangent.w;

    mat3 tbn = mat3(tangent, bitangent, normal);
    vec3 normal_mapping = texture(normalTexture, vertex_texcoord).rgb * 2. - 1.;
    normal = tbn * normal_mapping;

    fragment_color = normal / 2. + .5;
}
