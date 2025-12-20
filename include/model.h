#ifndef MODEL_H
#define MODEL_H
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

typedef struct {
    Mesh* meshes;
    int numMeshes;

    char* directory;
} Model;

void modelLoadModel(Model* model, char* path);
void modelDraw(Model* model, Shader* shader);

void modelProcessNode(Model* model, struct aiNode* node, const struct aiScene* scene);
Mesh modelProcessMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene);
Texture* modelLoadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName);

#endif
