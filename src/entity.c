#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "entity.h"

Entity* newEntity(char* model_path, char* vert_shader_path, char* frag_shader_path) {
    // Set up a shader for our backpack
    // FIXME: Find a way to globally refer to this shader
    Shader* shader = newShader(
      vert_shader_path,
      frag_shader_path
    );
    if (shader == NULL) {
        printf("Failed to initialize shader for entity %s, %s, %s\n", model_path, vert_shader_path, frag_shader_path);
        return NULL;
    }

    char backpackModelPath[] = "models/backpack/backpack.obj";
    Model* model = newModel(model_path);
    
    Entity* entity = malloc(sizeof(Entity));

    entity->model = model;
    entity->shader = shader;

    return entity;
}

// TODO: get view and projection
void entity_draw(Entity* entity) {

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
