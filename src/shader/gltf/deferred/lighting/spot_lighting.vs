#version 460 core

out vec2 vertex_texcoords;

vec2 vertices[] = {
    vec2(-1., -1.), vec2(-1., 1.), vec2(1., -1.),
    vec2(-1., 1.), vec2(1., -1.), vec2(1., 1.),
};

void main() {
    vertex_texcoords = vertices[gl_VertexID] / 2. + .5;

    gl_Position = vec4(vertices[gl_VertexID], 0., 1.);
}
