#version 460 core

uniform mat4 transform;

out vec3 vertex_direction;
out vec2 vertex_texcoords;

vec2 quad[] = {
    vec2(-1., -1.), vec2(-1.,  1.), vec2( 1., -1.),
    vec2( 1.,  1.), vec2( 1., -1.), vec2(-1.,  1.), 
};

void main() {
    vec2 q = quad[gl_VertexID];

    vertex_direction = normalize((transform * vec4(q.x, q.y / 16. * 9., 1., 0.)).xyz);
    vertex_texcoords = q * 5. + .5;

    gl_Position = vec4(q, 0., 1.);
}
