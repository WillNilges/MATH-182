#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
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

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform DirLight dirLight;

#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];

#define NR_SPOT_LIGHTS 1
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform Material material;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
            material.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,
                    TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,
                    TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,
                    TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance
                + light.quadratic * pow(distance, 2));

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,
                    TexCoords));

    vec3 specular = light.specular * spec * vec3(texture(material.specular,
                    TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
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
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 s = vec3(texture(material.specular, TexCoords));
        vec3 specular = light.specular * spec * s;

        diffuse *= intensity;
        specular *= intensity;

        // Calculate attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance *
                        light.quadratic * (distance * distance));

        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        return result;
    }
    else // else, use ambient light so scene isn't completely dark outside the spotlight.
        return (light.ambient * vec3(texture(material.diffuse, TexCoords)));
}

// TODO: Optimize this. We don't need to duplicate our calculations.
void main()
{
    vec3 viewPos = vec3(0.0);

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // phase 1: Directional lighting
    //result += CalcDirLight(dirLight, norm, viewDir);

    // phase 2: Point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    for (int i = 0; i < NR_SPOT_LIGHTS; i++)
    {
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}
