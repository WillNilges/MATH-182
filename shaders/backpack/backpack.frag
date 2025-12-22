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

//#define NR_MATERIALS 2 // There will probably be like 4 IDK
// TODO: Re-write your draw function to use multiple of these materials?

//uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

out vec4 FragColor;

vec3 CalcDirLight(sampler2D material, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
            0.5 * 128.0); // TODO: pass this value through somehow

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material,
                    TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material,
                    TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material,
                    TexCoords));

    return (ambient + diffuse + specular);
}

// TODO: Optimize this. We don't need to duplicate our calculations.
void main()
{
    vec3 viewPos = vec3(0.0);

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(1.0);

    // phase 1: Directional lighting
    //result += CalcDirLight(texture_diffuse1, dirLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}
