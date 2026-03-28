#version 330 core

in vec3 vertColor;

uniform vec3 objectColor;

out vec4 FragColor;
void main()
{
    FragColor = vec4(vertColor * objectColor, 1.0);
}
