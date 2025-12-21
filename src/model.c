#include "model.h"
#include "assimp/cimport.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "cglm/types.h"
#include "mesh.h"
#include "texture.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void modelLoadModel(Model* model, char* path)
{
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        return;
    }
    model->directory = path;
    model_processNode(model, scene->mRootNode, scene);
}

void model_draw(Model* model, Shader* shader)
{
    for (unsigned int i = 0; i < model->numMeshes; i++)
    {
        mesh_draw(&model->meshes[i], shader);
    }
}

void model_processNode(Model* model, struct aiNode* node, const struct aiScene* scene)
{
    // Process node's meshes (if any exist)

    // TODO: Allocate longer mesh array
    // XXX: Does it matter what order we render meshes in? Let's find out!
    // I might end up needing to somehow reverse this

    // Make a new mesh array that can fit the new meshes
    Mesh* newMeshes = malloc(sizeof(Mesh) * (node->mNumMeshes + model->numMeshes));

    // Copy the old meshes into the new mesh array
    // XXX: I might have to optimize this
    memcpy(newMeshes, model->meshes, model->numMeshes);

    // Create the new meshes ahead of the old meshes
    for(unsigned int i = model->numMeshes; i < model->numMeshes + node->mNumMeshes; i++)
    {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        newMeshes[i] = model_processMesh(model, mesh, scene);
    }

    // Update the number of meshes we have
    model->numMeshes += node->mNumMeshes;

    // Free the old array
    free(model->meshes);

    // Move the new mesh array to the object
    model->meshes = newMeshes;

    // Then process meshes for any children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        model_processNode(model, node->mChildren[i], scene);
    }
}

Mesh model_ProcessMesh(Model * model, struct aiMesh* mesh, const struct aiScene* scene)
{
    Vertex* vertices;
    size_t numVertices;

    unsigned int* indices;
    size_t numIndices;

    Texture* textures;
    size_t numTextures;

    // Allocate our vertex array
    vertices = malloc(sizeof(Vertex) * mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices[i].Position.x = mesh->mVertices[i].x;
        vertices[i].Position.y = mesh->mVertices[i].y;
        vertices[i].Position.z = mesh->mVertices[i].z;

        vertices[i].Normal.x = mesh->mNormals[i].x;
        vertices[i].Normal.y = mesh->mNormals[i].y;
        vertices[i].Normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            vec2s vec;
            vertices[i].TexCoords.x = mesh->mTextureCoords[0]->x;
            vertices[i].TexCoords.y = mesh->mTextureCoords[0]->y;
        }
        else 
        {
            vertices[i].TexCoords.x = 0.0f;
            vertices[i].TexCoords.y = 0.0f;
        }
    }

    // Figure out how many indices we have
    numIndices = 0;
    size_t currentIndex;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        numIndices += mesh->mFaces[i].mNumIndices;
    }
    indices = malloc(sizeof(unsigned int) * numIndices);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            //indices.push_back(face.mIndices[j]);
            indices[currentIndex] = face.mIndices[j];
            currentIndex++;
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        Texture* diffuseMaps = model_loadMaterialTextures(model, material, aiTextureType_DIFFUSE, MODEL_TEXTURE_DIFFUSE);
        //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        size_t numDiffuseMaps = aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);

        Texture* specularMaps = model_loadMaterialTextures(model, material, aiTextureType_SPECULAR, MODEL_TEXTURE_SPECULAR);
        size_t numSpecularMaps = aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
        //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        textures = malloc(sizeof(Texture) * (numDiffuseMaps + numSpecularMaps));

        memcpy(textures, diffuseMaps, numDiffuseMaps);
        memcpy(&textures[numDiffuseMaps], specularMaps, numSpecularMaps); // XXX: Will this work?
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

Texture* model_loadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName)
{
    Texture* textures;
    textures = malloc(sizeof(Texture) * aiGetMaterialTextureCount(mat, type));
    for(unsigned int i = 0; i < aiGetMaterialTextureCount(mat, type); i++)
    {
        struct aiString str;
        aiGetMaterialTexture(mat, type, 0, &str);
        Texture texture;
        char texturePath[strlen(str.data) + strlen(model->directory) + 2];
        snprintf(texturePath, strlen(str.data) + strlen(model->directory) + 2, "%s/%s", model->directory, str.data);
        texture.id = loadTexture(texturePath);
        texture.type = typeName;
        texture.path = str.data;
        textures[i] = texture;
    }

    return textures;
}
