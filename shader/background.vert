#version 330 core

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

varying vec2 fragment_uv;

void main() {
    fragment_uv = vertex_uv;
    gl_Position = vec4(vertex_pos, 1.0);
}