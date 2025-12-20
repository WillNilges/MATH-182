#include "mesh.h"
#include "shader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <string.h>

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

void meshDraw(Mesh* mesh, Shader* shader)
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

void meshSetup(Mesh* mesh)
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
