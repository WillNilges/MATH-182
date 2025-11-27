#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D awesomeTexture;

void main()
{
    //FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    FragColor = mix(texture(ourTexture, TexCoord * 2.0), texture(awesomeTexture, vec2(TexCoord.x * 2.0, TexCoord.y * 2.0)), 0.2);
}
