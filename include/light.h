#ifndef LIGHT_H
#define LIGHT_H
#include "cglm/types.h"

typedef struct {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DirLight;

typedef struct {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} PointLight;


typedef struct {
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
} SpotLight;



#endif
