#version __VERSION__

uniform vec3 color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(color), 1.0);
}