#ifndef SCENE_H
#define SCENE_H
#include "entity.h"
#include "light.h"
#include "shader.h"

/*
enum LightType {
 DIR,
 POINT,
 SPOT
};

typedef union {
  DirLight dirLight;
  PointLight pointLight;
  SpotLight spotLight;
} u_Light;

typedef struct {
  enum LightType type;
  u_Light light;
} Light;
*/

typedef struct {
  DirLight* dirLight; // This is only meant to be one light: THE SUN!

  PointLight** pointLights;
  size_t lenPointLights;

  SpotLight** spotLights;
  size_t lenSpotLights;
} Lighting;

void lighting_setQty(Lighting* lighting, Shader* shader);
void lighting_setLights(Lighting*lighting, Shader* shader);

typedef struct {
  Shader** shaders;
  size_t lenShaders;

  Lighting lighting;
  
  Entity** entities;
  size_t lenEntities;
} Scene;

Scene* newScene();

void scene_registerDirLight(Scene* scene, DirLight* dirLight);
void scene_registerPointLight(Scene* scene, PointLight* pointLight);
void scene_registerSpotLight(Scene* scene, SpotLight* spotLight);
void scene_registerShader(Scene* scene, Shader* shader);
void scene_registerEntity(Scene* scene, Entity* entity);

void scene_draw(Scene* scene, Camera* camera);

#endif
