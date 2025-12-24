#include "model.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assimp/types.h"
#include "cglm/types.h"
//#include "libgen.h"
#include "libgen.h"
#include "texture.h"

const char MODEL_MATERIAL_DOT[] = "texture_%s%d";

const char MODEL_TEXTURE_DIFFUSE[] = "diffuse";
const char MODEL_TEXTURE_SPECULAR[] = "specular";

Mesh* newMesh(Vertex* vertices, size_t numVertices, unsigned int* indices, size_t numIndices, Texture* textures, size_t numTextures)
{
    Mesh* mesh = malloc(sizeof(Mesh));
    mesh->vertices = vertices;
    mesh->numVertices = numVertices;
    mesh->indices = indices;
    mesh->numIndices = numIndices;
    mesh->textures = textures;
    mesh->numTextures = numTextures;

    mesh_setup(mesh);

    return mesh;
}

void mesh_draw(Mesh* mesh, Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    //printf("Num textures: %zu\n", mesh->numTextures);
    for (unsigned int i = 0; i < mesh->numTextures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Activate texture unit
        // retrieve texture number (N in diffuse_textureN)
        char* number;
        char* type = mesh->textures[i].type;
        //printf("Texture: %s, Type: %s\n", mesh->textures[i].path, type);
        if (strcmp(type, MODEL_TEXTURE_DIFFUSE) == 0)
        {
            size_t lenType = strlen(type);
            size_t lenNumber = snprintf(NULL, 0, "%u", diffuseNr);
            char shaderVarName[lenType + lenNumber + strlen(MODEL_MATERIAL_DOT)];
            snprintf(shaderVarName, sizeof(shaderVarName), MODEL_MATERIAL_DOT, type, diffuseNr);

            printf("setting int %s\n", shaderVarName);
            shaderSetInt(shader, shaderVarName, i);
            diffuseNr++;
        }
        else if (strcmp(type, MODEL_TEXTURE_SPECULAR) == 0)
        {
            size_t lenType = strlen(type);
            size_t lenNumber = snprintf(NULL, 0, "%u", specularNr);
            char shaderVarName[lenType + lenNumber + strlen(MODEL_MATERIAL_DOT)];
            snprintf(shaderVarName, sizeof(shaderVarName), MODEL_MATERIAL_DOT, type, specularNr);

            printf("setting int %s\n", shaderVarName);

            shaderSetInt(shader, shaderVarName, i);
            specularNr++;
        }
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    //printf("Indices: %zu\n", mesh->numIndices);

    // Draw mesh
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void mesh_setup(Mesh* mesh)
{
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * mesh->numVertices,
        &mesh->vertices[0],
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * mesh->numIndices,
        &mesh->indices[0],
        GL_STATIC_DRAW
    );

    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // vertex textrure coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Model* newModel(char* path)
{
    Model* model = malloc(sizeof(Model));

    model->meshes = NULL;
    model->numMeshes = 0;

    model->directory = NULL;

    model->texturesLoaded = NULL;
    model->numTexturesLoaded = 0;

    model_loadModel(model, path);

    return model;
}

void model_loadModel(Model* model, char* path)
{
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        return;
    }
    model->directory = dirname(path);
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
    model->meshes = realloc(model->meshes, sizeof(Mesh) * (node->mNumMeshes + model->numMeshes));

    // Create the new meshes ahead of the old meshes
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes[model->numMeshes + i] = *model_processMesh(model, mesh, scene);
    }

    // Update the number of meshes we have
    model->numMeshes += node->mNumMeshes;

    // Then process meshes for any children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        //printf("Processing child %d\n", i);
        model_processNode(model, node->mChildren[i], scene);
    }
}

Mesh* model_processMesh(Model * model, struct aiMesh* mesh, const struct aiScene* scene)
{
    Vertex* vertices;
    //size_t numVertices;

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
    size_t currentIndex = 0;
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
        size_t numDiffuseMaps = aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);

        Texture* specularMaps = model_loadMaterialTextures(model, material, aiTextureType_SPECULAR, MODEL_TEXTURE_SPECULAR);
        size_t numSpecularMaps = aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);

        textures = malloc(sizeof(Texture) * (numDiffuseMaps + numSpecularMaps));
        memcpy(textures, diffuseMaps, sizeof(Texture) * numDiffuseMaps);
        memcpy(&textures[numDiffuseMaps], specularMaps, sizeof(Texture) * numSpecularMaps); // XXX: Will this work?
        numTextures = numDiffuseMaps + numSpecularMaps;
    }

    Mesh* m = newMesh(vertices, mesh->mNumVertices, indices, numIndices, textures, numTextures);
    return m;
}

Texture* model_loadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName)
{
    size_t textureCount = aiGetMaterialTextureCount(mat, type);
    Texture* textures = malloc(sizeof(Texture) * textureCount);
    for (unsigned int i = 0; i < textureCount; i++)
    {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

        // Get the texture path
        size_t lenTexturePath = str.length + strlen(model->directory) + 2;
        char texturePath[lenTexturePath];
        snprintf(texturePath, lenTexturePath, "%s/%s", model->directory, str.data);

        //printf("Checking if we need to load texture %s\n", str.data);

        // Before doing the expensive step of loading the texture, check
        // if we've already loaded it into memory and use that copy if we can.
        bool skipLoading = false;
        for (unsigned int j = 0; j < model->numTexturesLoaded; j++)
        {
            if (model->texturesLoaded[j].type == typeName && strcmp(model->texturesLoaded[j].path, str.data) == 0)
            {
                textures[i].id = model->texturesLoaded[j].id;
                textures[i].type = model->texturesLoaded[j].type;
                textures[i].path = model->texturesLoaded[j].path;
                skipLoading = true;
                //printf("Not loading. %s = %s\n", model->texturesLoaded[j].path, str.data);
                break;
            }
        }

        if (skipLoading)
        {
            continue;
        }

        printf("We should load %s\n", str.data);

        // Set the texture
        textures[i].id = loadTexture(texturePath);
        textures[i].type = typeName;
        textures[i].path = str.data;

        // ...and update the known texture array
        model->texturesLoaded = realloc(model->texturesLoaded, sizeof(Texture) * (++model->numTexturesLoaded));
        model->texturesLoaded[model->numTexturesLoaded - 1] = textures[i];
    }

    return textures;
}
