#ifndef MESH_H
#define MESH_H

#include "cglm/types-struct.h"
#include "shader.h"
#include <stddef.h>

char* MODEL_TEXTURE_DIFFUSE = "texture_diffuse";
char* MODEL_TEXTURE_SPECULAR = "texture_specular";
char* MODEL_MATERIAL_DOT = "material.";

typedef struct {
    vec3s Position;
    vec3s Normal;
    vec2s TexCoords;
} Vertex;

typedef struct {
    unsigned int id;
    char* type;
} Texture;

typedef struct {
    Vertex* vertices;
    unsigned int numVertices;

    unsigned int* indices;
    unsigned int numIndices;

    Texture* textures;
    unsigned int numTextures;

    unsigned int VAO, VBO, EBO;
} Mesh;

Mesh* newMesh(Vertex* vertices, size_t numVertices, unsigned int* indices, size_t numIndices, Texture* textures, size_t numTextures);
void meshDraw(Mesh* mesh, Shader* shader);

void meshSetup(Mesh* mesh);

#endif
