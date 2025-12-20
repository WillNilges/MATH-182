#ifndef MODEL_H
#define MODEL_H

#include "cglm/types.h"
#include "shader.h"

char* MODEL_TEXTURE_DIFFUSE = "texture_diffuse";
char* MODEL_TEXTURE_SPECULAR = "texture_specular";
char* MODEL_MATERIAL_DOT = "material.";

typedef struct {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
} Vertex;

typedef struct {
    unsigned int id;
    char* type;
} Texture;

typedef struct {
    Vertex* vertices;
    unsigned int* indices;
    Texture* textures;

    unsigned int VAO, VBO, EBO;
} Mesh;

Mesh* NewMesh(Vertex* vertices, unsigned int* indices, Texture* textures);
void meshDraw(Mesh* mesh, Shader* shader);

void modelSetupMesh(Mesh* mesh);

#endif
