#include "light.h"
#include "camera.h"
#include "cglm/mat4.h"
#include "scene.h"
#include <stdio.h>

void dirLight_setDirection(DirLight* light, float x, float y, float z) {
  light->direction.x = x;
  light->direction.y = y;
  light->direction.z = z;
}

void dirLight_setAmbient(DirLight* light, float x, float y, float z) {
  light->ambient.x = x;
  light->ambient.y = y;
  light->ambient.z = z;
}

void dirLight_setDiffuse(DirLight* light, float x, float y, float z) {
  light->diffuse.x = x;
  light->diffuse.y = y;
  light->diffuse.z = z;
}

void dirLight_setSpecular(DirLight* light, float x, float y, float z) {
  light->specular.x = x;
  light->specular.y = y;
  light->specular.z = z;
}

