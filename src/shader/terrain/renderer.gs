// #version 460 core

// layout(triangles) in;

// layout(triangle_strip, max_vertices = 3) out;

// in VS_OUT {
//     vec3 vertex_normal;
//     vec3 vertex_position;
//     vec2 vertex_uv;
// } gs_in[];

// out vec3 vertex_normal;
// out vec2 vertex_uv;

// void do_vertex(int i) {
//     vertex_normal = gs_in[i].vertex_normal;
//     vertex_uv = gs_in[i].vertex_uv;
//     gl_Position = gl_in[i].gl_Position;
//     EmitVertex();
// }

// void main() {
//     do_vertex(0);
//     do_vertex(1);
//     do_vertex(2);
//     EndPrimitive();
// }
