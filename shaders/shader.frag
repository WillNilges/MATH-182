#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D awesomeTexture;

uniform float visibility;

void main()
{
    //FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    float scaleFactor = 1.0;
    FragColor = mix(texture(ourTexture, TexCoord * 2.0), texture(awesomeTexture, vec2(TexCoord.x * scaleFactor, TexCoord.y * scaleFactor)), visibility);
}
