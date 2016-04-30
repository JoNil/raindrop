#version 330 core

layout(points) in;
layout(points) out;
layout(max_vertices = 1) out;

in float size;

main() {
    
    gl_Position = gl_in[0].gl_Position.xyz
    EmitVertex();
    EndPrimitive();
    
}