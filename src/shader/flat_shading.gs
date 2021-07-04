#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vertex_position[];

out vec3 primitive_normal;

void main() {
    primitive_normal = normalize(cross(
        vertex_position[1] - vertex_position[0],
        vertex_position[2] - vertex_position[0]));

    gl_Position = gl_in[0].gl_Position;

    EmitVertex();

    primitive_normal = normalize(cross(
        vertex_position[1] - vertex_position[0],
        vertex_position[2] - vertex_position[0]));

    gl_Position = gl_in[1].gl_Position;

    EmitVertex(); 

    primitive_normal = normalize(cross(
        vertex_position[1] - vertex_position[0],
        vertex_position[2] - vertex_position[0]));
        
    gl_Position = gl_in[2].gl_Position;

    EmitVertex();

    EndPrimitive();
}
