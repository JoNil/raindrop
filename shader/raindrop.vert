#version 330 core

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in float size;
layout(location = 2) in vec2 vel;

out float pSize;
out vec2 pVel;

uniform mat4 MVP;

void main() {

    pSize = size;
    pVel = vel;

    gl_Position = MVP * vec4(vertex_pos, 0.0, 1.0);
}