#version __VERSION__

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 scaleUV;

out VERTEX_OUT {
    vec3 Normal;
    vec3 FragPos; 
    vec2 TexCoords;
    mat3 TBN;
} vertex_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertex_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vertex_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    if(length(scaleUV) == 0)
        vertex_out.TexCoords = aTexCoords;
    else
        vertex_out.TexCoords = (aTexCoords) * scaleUV;

   vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
   vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
   vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
   vertex_out.TBN = mat3(T, B, N);
} 