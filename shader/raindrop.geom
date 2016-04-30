#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 70) out;

in float pSize[];

const float PI = 3.1415926;

void main() {
    
    float angle;
    
    for (int i = 0; i <= 20; ++i) {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        

        for (int j = 0; j < 2; j++) {
            
            angle = 2 * PI / 20.0 * (i + j);
        
            vec4 position = gl_in[0].gl_Position;
            gl_Position = vec4(position.x + cos(angle) * pSize[0], position.y + sin(angle) * pSize[0], position.z, 1.0);
            EmitVertex();
        }
        
        EndPrimitive();
    }
}