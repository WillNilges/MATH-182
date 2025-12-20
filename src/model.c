#include "model.h"
#include "assimp/cimport.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "cglm/types.h"
#include "mesh.h"
#include <stdio.h>

void modelLoadModel(Model* model, char* path)
{
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        return;
    }
    model->directory = path;
    modelProcessNode(model, scene->mRootNode, scene);
}

void modelDraw(Model* model, Shader* shader)
{
    for (unsigned int i = 0; i < model->numMeshes; i++)
    {
        meshDraw(&model->meshes[i], shader);
    }
}

void modelProcessNode(Model* model, struct aiNode* node, const struct aiScene* scene)
{
    // Process node's meshes (if any exist)

    // TODO: Allocate longer mesh array


    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(modelProcessMesh(model, mesh, scene));
    }

    // Then process meshes for any children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        modelProcessNode(model, node->mChildren[i], scene);
    }
}

Mesh modelProcessMesh(Model * model, struct aiMesh* mesh, const struct aiScene* scene)
{
    Vertex* vertices;
    size_t numVertices;

    unsigned int* indices;
    size_t numIndices;

    Texture* textures;
    size_t numTextures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        vertex.Normal.x = mesh->mNormals[i].x;
        vertex.Normal.y = mesh->mNormals[i].y;
        vertex.Normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            vec2s vec;
            vertex.TexCoords.x = mesh->mTextureCoords[0]->x;
            vertex.TexCoords.y = mesh->mTextureCoords[0]->y;
        }
        else 
        {
            vertex.TexCoords.x = 0.0f;
            vertex.TexCoords.y = 0.0f;
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        Texture* diffuseMaps = modelLoadMaterialTextures(model, material, aiTextureType_DIFFUSE, MODEL_TEXTURE_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.begin());
        Texture* specularMaps = modelLoadMaterialTextures(model, material, aiTextureType_SPECULAR, MODEL_TEXTURE_SPECULAR);
    }

    Mesh m = {
        vertices: vertices,
        numVertices: numVertices,

        indices: indices,
        numIndices: numIndices,

        textures: textures,
        numTextures: numTextures,
    };

    return m;
}

Texture* modelLoadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName)
{
    Texture* textures;
    for(unsigned int i = 0; i < aiGetMaterialTextureCount(mat, type); i++)
    {
        struct aiString str;
        aiGetMaterialTexture(mat, type, &str);
        Texture texture;
        texture.id = TextureFromFile(str.data, model->directory); // TODO: Implement this
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture); // FIXME: make this c compatible
    }

    return textures;
}
