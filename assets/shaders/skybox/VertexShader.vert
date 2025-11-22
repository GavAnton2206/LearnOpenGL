#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    mat4 viewNoTranslation = mat4(mat3(view));

    TexCoords = aPos;
    gl_Position = projection * viewNoTranslation * vec4(aPos, 1.0);
}  

