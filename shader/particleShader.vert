#version 130

attribute vec3 vertex_pos;
attribute vec2 vertex_tex;

varying vec3 pos;
varying vec2 tex;

void main() {
    pos = vertex_pos;
    tex = vertex_tex;
    gl_Position = vec4(vertex_pos, 1.0);
}