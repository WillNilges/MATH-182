#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff; // Angle of the cone of the spotlight
    float outerCutOff; // Angle of the cone of the spotlight

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Properties of the light
uniform Light light;

uniform Material material;

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    if (theta > light.outerCutOff)
    {
        // do lighting calculations

        // Spotlight
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 viewPos = vec3(0.0);
        // Ambient
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

        // Diffuse
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 s = vec3(texture(material.specular, TexCoords));
        vec3 specular = light.specular * spec * s;

        diffuse *= intensity;
        specular *= intensity;

        // Calculate attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance *
                        light.quadratic * (distance * distance));

        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else // else, use ambient light so scene isn't completely dark outside the spotlight.
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
}
