 #version 330 core

varying vec3 pos;
varying vec2 tex;

void main()
{
    vec3 color = vec3(191.0/255.0, 0.0, 1.0);

    vec2 local = (tex - 0.5) * 2.0;

    float r = sqrt(local.x*local.x + local.y*local.y);
	r = clamp(r, 0.0, 1.0);
	float alpha = 1.5;
	alpha*= pow(1.0 - r, 2.0);

    gl_FragColor = vec4(color, alpha);
}