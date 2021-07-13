#version 460 core

layout(triangles) in;

layout(line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 vertex_normal;
} gs_in[];

void main() {
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(gs_in[0].vertex_normal, 0.f);
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(gs_in[1].vertex_normal, 0.f);
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + vec4(gs_in[2].vertex_normal, 0.f);
    EmitVertex();
    EndPrimitive();
}
