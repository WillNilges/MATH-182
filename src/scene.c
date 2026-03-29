#include "scene.h"
#include "entity.h"
#include <stdlib.h>

Scene* newScene()
{
  Scene* scene = malloc(sizeof(Scene));
  scene->shaders = malloc(sizeof(Shader));
  scene->lighting = malloc(sizeof(Lighting));
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

    // Do the directional lighting calculations
    // Process lighting information using provided camera
    vec3 viewspaceLightDir;
    glm_mat4_mulv3(camera->view, scene->lighting->dirLight->direction.raw, 1.0, viewspaceLightDir);
    
    shaderSetVec3(s, "dirLight.direction", viewspaceLightDir);
    shaderSetVec3(s, "dirLight.ambient", scene->lighting->dirLight->ambient.raw);
    shaderSetVec3(s, "dirLight.diffuse", scene->lighting->dirLight->diffuse.raw);
    shaderSetVec3(s, "dirLight.specular", scene->lighting->dirLight->specular.raw);

    // TODO: Set up other lights
    // XXX: Would it be nice to pass the lights into some kind of generic function per shader?

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
