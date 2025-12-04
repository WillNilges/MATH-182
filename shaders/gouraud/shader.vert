#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec3 vertColor;

void main()
{
    // Lighting calculations, in view space, on the vertex shader.
    // who woulda thought!?
    vec3 viewFragPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(view * model))) * aNormal;
    vec3 viewLightPos = vec3(view * vec4(lightPos, 1.0));
    vec3 viewViewPos = vec3(0.0);

    // Diffuse
    vec3 norm = normalize(Normal);
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

    vertColor = ambient + diffuse + specular;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
