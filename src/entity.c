#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "entity.h"

Entity* newEntity(char* model_path, Shader* shader) {
    Model* model = newModel(model_path);
    
    Entity* entity = malloc(sizeof(Entity));

    entity->model = model;
    entity->shader = shader;

    return entity;
}

// TODO: get view and projection
void entity_draw(Entity* entity, Camera* camera, DirLight* dirLight) {
  shaderUse(entity->shader);
  /*
  shaderSetMat4v(entity->shader, "view", view);
  shaderSetMat4v(entity->shader, "projection", projection);
  shaderSetVec3(entity->shader, "dirLight.direction", dirLight.direction);
  shaderSetVec3(entity->shader, "dirLight.ambient", dirLight.ambient);
  shaderSetVec3(entity->shader, "dirLight.diffuse", dirLight.diffuse);
  shaderSetVec3(entity->shader, "dirLight.specular", dirLight.specular);
  shaderSetMat4v(entity->shader, "model", entity->transform);
  */

  model_draw(entity->model, entity->shader);
 
}
