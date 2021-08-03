#version 460 core

uniform mat4 mvp;
uniform mat4 normal_transform;

// layout(location = 0) in vec3 color_rgb;
layout(location = 0) in vec3 normal3;
layout(location = 1) in vec3 position3;
layout(location = 2) in vec2 texcoords2;

out vec3 vertex_normal3;
out vec2 vertex_texcoords2;

void main() {
    vertex_normal3 = normalize((normal_transform * vec4(normal3, 0.f)).xyz);
    vertex_texcoords2 = texcoords2;
    
    gl_Position = mvp * vec4(position3, 1.f);
}
