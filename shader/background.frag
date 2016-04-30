#version 330 core

in vec2 fragment_uv;

uniform sampler2D tex;

void main()
{
    gl_FragColor = texture2D(tex, fragment_uv);
}