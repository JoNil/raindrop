#version 330 core

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in float size;

out float pSize;

void main() {

    pSize = size;

    gl_Position = vec4(vertex_pos, 0.0, 1.0);
}