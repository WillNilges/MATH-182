#ifndef LIGHT_H
#define LIGHT_H
#include "cglm/types-struct.h"
#include "cglm/types.h"

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
