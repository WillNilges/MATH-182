#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "entity.h"
#include "cglm/types-struct.h"

Entity* newEntity(char* model_path, Shader* shader) {
    Model* model = newModel(model_path);
    
    Entity* entity = malloc(sizeof(Entity));

    entity->model = model;
    entity->shader = shader;

    entity->position.x = 0.0f;
    entity->position.y = 0.0f;
    entity->position.z = 0.0f;

    return entity;
}

// TODO: get view and projection
void entity_draw(Entity* entity, Camera* camera, DirLight* dirLight) {
  // Activate this entity's shader
  shaderUse(entity->shader);
  
  // Translate the model in world(?) space
  mat4 model;
  glm_mat4_identity(model);
  glm_translate(model, entity->position.raw);
  
  // Process lighting information using provided camera
  vec3 viewspaceLightDir;
  glm_mat4_mulv3(camera->view, dirLight->direction.raw, 1.0, viewspaceLightDir);

  shaderUse(entity->shader);
  shaderSetMat4v(entity->shader, "view", camera->view);
  shaderSetMat4v(entity->shader, "projection", camera->projection);
  shaderSetVec3(entity->shader, "dirLight.direction", viewspaceLightDir);
  shaderSetVec3(entity->shader, "dirLight.ambient", dirLight->ambient.raw);
  shaderSetVec3(entity->shader, "dirLight.diffuse", dirLight->diffuse.raw);
  shaderSetVec3(entity->shader, "dirLight.specular", dirLight->specular.raw);
  shaderSetMat4v(entity->shader, "model", model);

  model_draw(entity->model, entity->shader);
 
}
