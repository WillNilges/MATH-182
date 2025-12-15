#ifndef LIGHT_H
#define LIGHT_H
#include "cglm/types.h"

typedef struct {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} PointLight;

#endif
