#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 60) out;

in float pSize[];

out vec3 Position;
out vec3 normal;
out vec2 texCoord;


const float PI = 3.1415926;

void main() {
    
    float angle;
    vec4 inputPosition = gl_in[0].gl_Position;
    
    for (int i = 0; i < 20; ++i) {
        Position = vec3(inputPosition);
        gl_Position = inputPosition;
        texCoord = vec2((inputPosition.x + 1)/ 2, (inputPosition.y + 1)/2);
        normal = vec3(0, 0, 1.0);
        EmitVertex();

        for (int j = 0; j < 2; j++) {
            
            angle = 2 * PI / 20.0 * (i + j);
            
            Position = vec3(inputPosition.x + cos(angle) * pSize[0],
                           inputPosition.y + sin(angle) * pSize[0],
                           inputPosition.z);
            
            gl_Position = vec4(Position, 1.0);
            
            
            float t = (inputPosition.y + sin((3.1415926 - angle) * 2 + angle) * pSize[0]);
                          
            texCoord = vec2(((Position.x + 1)/ 2), (t + 1)/2);
            
            
            
            normal = normalize(Position - vec3(inputPosition));
            
            EmitVertex();
        }
        
        EndPrimitive();
    }
}