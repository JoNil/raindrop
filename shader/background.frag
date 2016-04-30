#version 130

varying vec2 fragment_uv;

void main()
{
    vec3 color = vec3(0.3, 0.2, 1.0);
    gl_FragColor = vec4(color, 1.0);
}