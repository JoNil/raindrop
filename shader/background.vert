#version 330 core

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

out vec2 fragment_uv;

uniform vec2 offset;

void main() {
    const float scale = 1.0;

    vec2 uv_in = scale * (vertex_uv + vec2(offset.x, offset.y));

    fragment_uv = vec2(uv_in.x, 1.0 - uv_in.y);
    gl_Position = vec4(vertex_pos, 1.0);
}