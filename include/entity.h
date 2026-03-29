
// enum type?

// shaders
// model
// transform

// a level/scene could have lighting information

#ifndef ENTITY_H
#define ENTITY_H
//#include "cglm/types.h"
#include "model.h"

Shader* loaded_shaders;

typedef struct {
  Model* model;
  Shader* shader;
} Entity;

Entity* newEntity(char* model_path, char* vert_shader_path, char* frag_shader_path);
void entity_draw(Entity* entity);

#endif
