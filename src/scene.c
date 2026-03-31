#include "scene.h"
#include "cglm/mat3.h"
#include "entity.h"
#include "light.h"
#include "shader.h"
#include <stdlib.h>


void lighting_setCounts(Lighting* lighting, Shader* shader)
{
      shaderSetInt(shader, "pointLightCount", (int)(lighting->lenPointLights));
      shaderSetInt(shader, "spotLightCount", (int)(lighting->lenSpotLights));
}

void lighting_setLights(Lighting* lighting, Shader* shader) {
  
}

Scene* newScene()
{
  Scene* scene = malloc(sizeof(Scene));
  scene->shaders = malloc(sizeof(Shader));
  scene->lighting = malloc(sizeof(Lighting));
  scene->lighting->dirLight = malloc(sizeof(DirLight));
  scene->lighting->pointLights = malloc(sizeof(PointLight));
  scene->lighting->spotLights = malloc(sizeof(SpotLight));
  scene->entities = malloc(sizeof(Entity));
  return scene;
}

void scene_registerDirLight(Scene* scene, DirLight* dirLight)
{
  scene->lighting->dirLight = dirLight;
}

// FIXME: Allow him to remove lights n shit
void scene_registerPointLight(Scene* scene, PointLight* pointLight)
{
  scene->lighting->pointLights = realloc(
    scene->lighting->pointLights,
    sizeof(PointLight) * (++scene->lighting->lenPointLights)
  );

  // FIXME: I feel like this is going to leak memory
  scene->lighting->pointLights[scene->lighting->lenPointLights - 1] = *pointLight;
}

void scene_registerSpotLight(Scene* scene, SpotLight* spotLight)
{
  scene->lighting->spotLights = realloc(
    scene->lighting->spotLights,
    sizeof(SpotLight) * (++scene->lighting->lenSpotLights)
  );

  // FIXME: I feel like this is going to leak memory
  scene->lighting->spotLights[scene->lighting->lenSpotLights - 1] = *spotLight;
}

void scene_registerShader(Scene* scene, Shader* shader)
{
  scene->shaders = realloc(
    scene->shaders,
    sizeof(Shader) * (++scene->lenShaders)
  );

  // FIXME: I feel like this is going to leak memory
  scene->shaders[scene->lenShaders - 1] = *shader;
}

void scene_registerEntity(Scene* scene, Entity* entity)
{
  scene->entities = realloc(
    scene->entities,
    sizeof(Entity) * (++scene->lenEntities)
  );

  // FIXME: I feel like this is going to leak memory
  scene->entities[scene->lenEntities - 1] = *entity;
}

void scene_draw(Scene* scene, Camera* camera)
{
  // For each shader
  // Build a list of entities that want to use this shader
  // activate the shader
  // set up the lights
  // draw the entities
  // Optimization: Only build that list once on startup and only change it when needed.

  for (int i = 0; i < scene->lenShaders; i++)
  {
    Shader* s = &(scene->shaders[i]);

    // Activate this shader
    shaderUse(s);

    // Pass the camera matricies in the shader
    shaderSetMat4v(s, "view", camera->view);
    shaderSetMat4v(s, "projection", camera->projection);

    // Set up lights

    // Tell the shader how many lights we have
    lighting_setCounts(scene->lighting, s);

    // Set up the dir light if one exists
    shader_loadDirLight(s, scene->lighting->dirLight, camera);

    // Set up point lights
    for (unsigned int j = 0; j < scene->lighting->lenPointLights; j++) {
      shader_loadPointLight(s, &(scene->lighting->pointLights[j]), j, camera);
    }

    // Set up spot lights
    // Add the flashlight info to the shader
    shaderSetInt(s, "spotLightCount", (int)(scene->lighting->lenSpotLights));
    for (unsigned int j = 0; j < scene->lighting->lenSpotLights; j++)
    {
      shader_loadSpotLight(s, &(scene->lighting->spotLights[j]), j, camera);
    }
    
    for (int j = 0; j < scene->lenEntities; j++)
    {
      Entity* e = &(scene->entities[j]);
      if (e->shader->ID == s->ID)
      {
        entity_draw(e, camera);
      }
    }
  }
}
