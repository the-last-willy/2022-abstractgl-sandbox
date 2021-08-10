#version 460 core

uniform sampler2D albedo_texture;

in vec2 vertex_texccords;

out vec4 fragment_rgba;

void main() {
    fragment_rgba = texture(albedo_texture, vertex_texccords);
}
