#version 330 core
in vec3 ourColor;
out vec4 FragColor;

uniform vec1 greenFactor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, greenFactor, 1.0f);
    //FragColor = vec4(ourColor, 1.0);
}
