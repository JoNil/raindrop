#version 330 core

in vec3 Position;
in vec3 normal;
in vec2 texCoord;

const vec3 LP = vec3(0.45, 1.5, 3.0);

uniform sampler2D tex;

const float shininess = 20.0;

void main()
{
    vec3 lightDir = normalize(LP - Position);
    vec3 viewDir  = normalize(-Position);
            
    float lightIntensity = 0.9f/length(lightDir);
    lightDir = normalize(lightDir);

    vec2 invUV = vec2(texCoord.s, 1.0-texCoord.t);
    
    vec3 texcolor = texture( tex, invUV ).rgb;
    vec3 white = vec3(1.0f, 1.0f, 1.0f);

    //Diffuse part-----------
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * white * lightIntensity;

    //specular part-------------
    vec3 H = normalize(lightDir + viewDir);
    float NdH = max(dot(H, normal), 0.0);
    float spec = pow(NdH, shininess);
    vec3 specular = spec * white;

    // Ambient-------------
    vec3 ambient = 0.8 * lightIntensity * white * texcolor * lightIntensity;
    
    vec3 resultLight = ambient + diffuse + specular;
    gl_FragColor = vec4(resultLight, 0.5);
}