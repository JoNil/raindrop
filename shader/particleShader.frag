 #version 330 core

varying vec3 pos;
varying vec2 tex;

void main()
{
    vec3 color = vec3(0.8, 0.0, 1.0);
    gl_FragColor = vec4(color, 1.0);
}