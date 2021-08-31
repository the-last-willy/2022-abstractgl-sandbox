#version 460 core

uniform mat4 joint_matrices[32];

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normal_transform;

layout(location = 0) in vec4 JOINTS_0;
layout(location = 1) in vec3 NORMAL;
layout(location = 2) in vec3 POSITION;
layout(location = 3) in vec4 TANGENT;
layout(location = 4) in vec2 TEXCOORD_0;
layout(location = 5) in vec4 WEIGHTS_0;

out vec3 vertex_normal;
out vec3 vertex_position;
out vec4 vertex_tangent;
out vec2 vertex_texcoord;

void main() {
    mat4 skinning = WEIGHTS_0[0] * joint_matrices[int(JOINTS_0[0])]
    + WEIGHTS_0[1] * joint_matrices[int(JOINTS_0[1])]
    + WEIGHTS_0[2] * joint_matrices[int(JOINTS_0[2])]
    + WEIGHTS_0[3] * joint_matrices[int(JOINTS_0[3])];

    mat4 normal_skinning = mat4(
        skinning[0], skinning[1], skinning[2], vec4(0., 0., 0., 1.));
    normal_skinning = transpose(inverse(normal_skinning));

    vertex_normal = normalize((normal_transform * normal_skinning * vec4(NORMAL, 0.)).xyz);
    vertex_position = (mv * skinning * vec4(POSITION, 1.)).xyz;
    vertex_tangent = vec4(
        normalize((normal_transform * normal_skinning * vec4(TANGENT.xyz, 0.)).xyz),
        TANGENT.w);
    vertex_texcoord = TEXCOORD_0;

    gl_Position = mvp * skinning * vec4(POSITION, 1.);
}
