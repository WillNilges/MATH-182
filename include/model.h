#ifndef MODEL_H
#define MODEL_H
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stddef.h>

#include "cglm/types-struct.h"
#include "shader.h"

typedef struct {
    vec3s Position;
    vec3s Normal;
    vec2s TexCoords;
} Vertex;

typedef struct {
    unsigned int id;
    char* type;
    char* path;
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
void mesh_draw(Mesh* mesh, Shader* shader);

void mesh_setup(Mesh* mesh);

typedef struct {
    Mesh* meshes;
    int numMeshes;

    char* directory;

    Texture* texturesLoaded;
    size_t numTexturesLoaded;
} Model;

Model* newModel(char* path);
void model_loadModel(Model* model, char* path);
void model_draw(Model* model, Shader* shader);

void model_processNode(Model* model, struct aiNode* node, const struct aiScene* scene);
Mesh model_processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene);
Texture* model_loadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName);

#endif
