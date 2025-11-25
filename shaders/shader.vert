#version 330 core
layout(location = 0) in vec3 aPos;

out vec4 vertexPosition;

uniform float offset;

void main()
{
    gl_Position = vec4(aPos.x + offset, aPos.y * -1.0, aPos.z, 1.0);
    vertexPosition = gl_Position;
}
