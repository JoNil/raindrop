#version 330 core

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

out vec2 fragment_uv;

uniform vec2 offset;

void main() {
    fragment_uv = vertex_uv + 0.1 * offset;
    gl_Position = vec4(vertex_pos, 1.0);
}