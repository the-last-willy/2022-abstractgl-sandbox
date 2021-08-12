#version 460 core

uniform sampler2D baseColorTexture;

in vec2 vertex_texcoord;

out vec4 fragment_color;

void main() {
    fragment_color = texture(baseColorTexture, vertex_texcoord).rgba;
}
