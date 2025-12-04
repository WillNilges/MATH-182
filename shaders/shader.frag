#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 view;

out vec4 FragColor;

void main()
{
    vec3 viewFragPos = vec3(view * vec4(FragPos, 1.0));
    vec3 viewNormal = Normal;
    vec3 viewLightPos = vec3(view * vec4(lightPos, 1.0));
    vec3 viewViewPos = vec3(0.0);

    // Diffuse
    vec3 norm = normalize(viewNormal);
    vec3 lightDir = normalize(viewLightPos - viewFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewViewPos - viewFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    int shininess = 256;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
