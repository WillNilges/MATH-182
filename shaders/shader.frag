#version 330 core
out vec4 FragColor;

uniform float greenFactor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, greenFactor, 1.0f);
    //FragColor = vec4(ourColor, 1.0);
}
