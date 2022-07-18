#version 450 core

uniform mat4 object_to_clip;
uniform mat4 object_to_world_normal;
uniform mat4 object_to_world_position;

in vec3 a_normal;
in vec3 a_position;
in vec3 a_texcoords0;

out vec3 v_texcoords0;
out vec3 v_world_normal;
out vec3 v_world_position;

void main() {
    v_texcoords0 = a_texcoords0;
    v_world_normal = (object_to_world_normal * vec4(a_normal, 0.)).xyz;
    v_world_position = (object_to_world_position * vec4(a_position, 1.)).xyz;

    gl_Position = object_to_clip * vec4(a_position, 1.);
}
