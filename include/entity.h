
// enum type?

// shaders
// model
// transform

// a level/scene could have lighting information

#ifndef ENTITY_H
#define ENTITY_H
//#include "cglm/types.h"
#include "light.h"
#include "model.h"

extern mat4 view;
extern mat4 projection;

extern DirLight dirLight;

//extern Shader* loaded_shaders;

typedef struct {
  Model* model;
  Shader* shader;
  mat4 transform;
} Entity;

Entity* newEntity(char* model_path, char* vert_shader_path, char* frag_shader_path);
void entity_draw(Entity* entity);

#endif
