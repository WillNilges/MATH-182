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

void entity_draw(Entity* entity, Camera* camera) {
  // Activate this entity's shader
  shaderUse(entity->shader);
  
  // Translate the model in world(?) space
  mat4 model;
  glm_mat4_identity(model);
  glm_translate(model, entity->position.raw);

  shaderSetMat4v(entity->shader, "model", model);

  model_draw(entity->model, entity->shader);
 
}
