#version 460 core

uniform sampler2D baseColorTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D metallicRoughnessTexture;
uniform sampler2D occlusionTexture;
uniform sampler2D normalTexture;

uniform float alphaCutoff = 0.5;
uniform int alphaMode = 0; // {BLEND, CUTOFF, OPAQUE}.
uniform vec4 baseColorFactor = vec4(1., 1., 1., 1.);

in vec3 vertex_normal;
in vec3 vertex_position;
in vec4 vertex_tangent;
in vec2 vertex_texcoord;

layout(location = 0) out vec3 albedo_texture;
layout(location = 1) out vec3 emissive_texture;
layout(location = 2) out vec3 metallic_roughness_texture;
layout(location = 3) out vec3 normal_texture;
layout(location = 4) out float occlusion_texture;
layout(location = 5) out vec3 position_texture;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 tangent = normalize(vertex_tangent.xyz);

    vec3 bitangent = vertex_tangent.w * cross(normal, tangent);

    mat3 tbn = mat3(tangent, bitangent, normal);
    vec3 normal_mapping = texture(normalTexture, vertex_texcoord).rgb * 2. - 1.;
    // Tangent space to view space.
    normal = tbn * normal_mapping;

    {
        vec4 albedo = baseColorFactor * texture(baseColorTexture, vertex_texcoord);
        if(alphaMode == 0) {

        } else if(alphaMode == 1) {
            if(albedo.a < alphaCutoff) {
                discard;
            } else {
                albedo_texture = albedo.rgb;
            }
        } else if(alphaMode == 2) {
            albedo_texture = albedo.rgb;
        }
    }
    
    emissive_texture = texture(emissiveTexture, vertex_texcoord).rgb;
    metallic_roughness_texture = texture(metallicRoughnessTexture, vertex_texcoord).rgb;
    normal_texture = normal;
    occlusion_texture = texture(occlusionTexture, vertex_texcoord).r;
    position_texture = vertex_position;
}
