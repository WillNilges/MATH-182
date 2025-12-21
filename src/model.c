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

#include "cglm/types.h"
#include "texture.h"

const char* MODEL_TEXTURE_DIFFUSE = "texture_diffuse";
const char* MODEL_TEXTURE_SPECULAR = "texture_specular";
const char* MODEL_MATERIAL_DOT = "material.";

Mesh* newMesh(Vertex* vertices, size_t numVertices, unsigned int* indices, size_t numIndices, Texture* textures, size_t numTextures)
{
    Mesh* mesh = malloc(sizeof(Mesh));
    mesh->vertices = vertices;
    mesh->numVertices = numVertices;
    mesh->indices = indices;
    mesh->numIndices = numIndices;
    mesh->textures = textures;
    mesh->numTextures = numTextures;

    return mesh;
}

void mesh_draw(Mesh* mesh, Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < mesh->numTextures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Activate texture unit
        // retrieve texture number (N in diffuse_textureN)
        char* number;
        char* type = mesh->textures[i].type;
        if (strcmp(type, MODEL_TEXTURE_DIFFUSE) == 0)
        {
            size_t lenType = strlen(type);
            size_t lenNumber = snprintf(NULL, 0, "%u", diffuseNr);
            char shaderVarName[lenType + lenNumber + strlen(MODEL_MATERIAL_DOT)];
            snprintf(shaderVarName, sizeof(shaderVarName), MODEL_MATERIAL_DOT, type, diffuseNr);
            shaderSetFloat(shader, shaderVarName, i);
            diffuseNr++;
        }
        else if (strcmp(type, MODEL_TEXTURE_SPECULAR) == 0)
        {
            size_t lenType = strlen(type);
            size_t lenNumber = snprintf(NULL, 0, "%u", specularNr);
            char shaderVarName[lenType + lenNumber + strlen(MODEL_MATERIAL_DOT)];
            snprintf(shaderVarName, sizeof(shaderVarName), MODEL_MATERIAL_DOT, type, specularNr);
            shaderSetFloat(shader, shaderVarName, i);
            specularNr++;
        }
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

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
        sizeof(*mesh->vertices),
        &mesh->vertices[0],
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(*mesh->indices),
        &mesh->indices[0],
        GL_STATIC_DRAW
    );

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // vertex textrure coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

Model* newModel(char* path)
{
    Model* model = malloc(sizeof(Model));

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

Mesh model_processMesh(Model * model, struct aiMesh* mesh, const struct aiScene* scene)
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
    // XXX: The Learnopengl optimization is going to fuck up my memory management
    Texture* textures;
    textures = malloc(sizeof(Texture) * aiGetMaterialTextureCount(mat, type));
    for(unsigned int i = 0; i < aiGetMaterialTextureCount(mat, type); i++)
    {
        struct aiString str;
        aiGetMaterialTexture(mat, type, 0, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        Texture texture;
        char texturePath[strlen(str.data) + strlen(model->directory) + 2];
        snprintf(texturePath, strlen(str.data) + strlen(model->directory) + 2, "%s/%s", model->directory, str.data);
        bool skipLoading = false;
        for (unsigned int j = 0; j < model->numTexturesLoaded; j++)
        {
            if (strcmp(model->texturesLoaded[j].path, str.data) == 0)
            {
                textures[i] = model->texturesLoaded[j];
                skipLoading = true;
                break;
            }
        }
        if (!skipLoading)
        {
            texture.id = loadTexture(texturePath);
            texture.type = typeName;
            texture.path = str.data;
            textures[i] = texture;
        }
    }

    return textures;
}
