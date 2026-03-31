#ifndef LIGHT_H
#define LIGHT_H
#include "camera.h"
#include "cglm/types-struct.h"
#include "shader.h"

// This is where we define our lighting standards
// Any shader that wants lights needs to adhere to this
// convention
// ex. pointLights[3].position

#define LIGHT_FMT "%s.%s"
#define MULTI_LIGHT_FMT "%s[%u].%s"

#define DIR_LIGHT "dirLight"
#define DIR_LIGHT_DIRECTION "direction"
#define DIR_LIGHT_AMBIENT "ambient"
#define DIR_LIGHT_DIFFUSE "diffuse"
#define DIR_LIGHT_SPECULAR "specular"

#define POINT_LIGHTS "pointLights"
#define POINT_LIGHTS_POSITION "position"
#define POINT_LIGHTS_CONSTANT "constant"
#define POINT_LIGHTS_LINEAR "linear"
#define POINT_LIGHTS_QUADRATIC "quadratic"
#define POINT_LIGHTS_AMBIENT "ambient"
#define POINT_LIGHTS_DIFFUSE "diffuse"
#define POINT_LIGHTS_SPECULAR "specular"

#define SPOT_LIGHTS "pointLights"
#define SPOT_LIGHTS_POSITION "position"
#define SPOT_LIGHTS_DIRECTION "direction"
#define SPOT_LIGHTS_CUTOFF "cutOff"
#define SPOT_LIGHTS_OUTER_CUTOFF "outerCutOff"
#define SPOT_LIGHTS_AMBIENT "ambient"
#define SPOT_LIGHTS_DIFFUSE "diffuse"
#define SPOT_LIGHTS_SPECULAR "specular"
#define SPOT_LIGHTS_CONSTANT "constant"
#define SPOT_LIGHTS_LINEAR "linear"
#define SPOT_LIGHTS_QUADRATIC "quadratic"


typedef struct {
    vec3s direction;

    vec3s ambient;
    vec3s diffuse;
    vec3s specular;
} DirLight;

void dirLight_setDirection(DirLight* light, float x, float y, float z);
void dirLight_setAmbient(DirLight* light, float x, float y, float z);
void dirLight_setDiffuse(DirLight* light, float x, float y, float z);
void dirLight_setSpecular(DirLight* light, float x, float y, float z);

void dirLight_setInShader(DirLight* light, Camera* camera, Shader* shader);

typedef struct {
    vec3s position;

    float constant;
    float linear;
    float quadratic;

    vec3s ambient;
    vec3s diffuse;
    vec3s specular;
} PointLight;



typedef struct {
    vec3s position;
    vec3s direction;
    float cutOff; // Angle of the cone of the spotlight
    float outerCutOff; // Angle of the cone of the spotlight

    vec3s ambient;
    vec3s diffuse;
    vec3s specular;

    float constant;
    float linear;
    float quadratic;
} SpotLight;

#endif
