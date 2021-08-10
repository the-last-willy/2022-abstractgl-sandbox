#version 460 core

out vec2 vertex_texcoords;

vec2 vertices[] = {
    vec2(  -1., -100.),
    vec2(+100.,    0.),
    vec2(  -1., +100.),
};

void main() {
    vertex_texcoords = vertices[gl_VertexID];

    gl_Position = vec4(vertices[gl_VertexID], 0., 1.);
}
