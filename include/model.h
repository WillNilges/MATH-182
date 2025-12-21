#ifndef MODEL_H
#define MODEL_H
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stddef.h>

#include "mesh.h"

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
