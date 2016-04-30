#version 330 core

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 vertex_tex;

out vec3 pos;
out vec2 tex;

void main() {
    pos = vertex_pos;
    tex = vertex_tex;
    gl_Position = vec4(vertex_pos, 1.0);
}