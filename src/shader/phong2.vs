#version 460

uniform mat4 mvp;

layout(location = 0) in vec3 position;


void main() {
    gl_Position = mvp * vec4(position.xy, position.z, 1.f);
}
