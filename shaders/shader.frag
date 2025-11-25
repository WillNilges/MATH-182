#version 330 core
in vec4 vertexPosition;
out vec4 FragColor;

uniform float greenFactor;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, greenFactor, 1.0f);
    //FragColor = vec4(ourColor, 1.0);
    FragColor = vertexPosition;
}

/*
The bottom left of our triangle is black because those values are closer to 0.
Red seems to be right, green seems to be up. So closer to the center (or negative,
I guess) is black.
*/
