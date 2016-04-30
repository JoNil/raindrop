#version 330 core

in vec3 pos;
in vec2 tex;

void main()
{
    vec3 color = vec3(0.0, 0.0, 1.0);
    gl_FragColor = vec4(color, 1.0);
}