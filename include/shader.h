#ifndef SHADER_H
#define SHADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

typedef struct {
    unsigned int ID;
    char* vertexPath;
    char* fragmentPath;
} Shader;

char* getShaderSourceFromFile(const char* filePath);
unsigned int compileShaderProgram(char* vertexPath, char* fragmentPath);

Shader* newShader(char* vertexPath, char* fragmentPath);
void shaderUse(Shader* shader);
void shaderSetInt(Shader* shader, const char* name, int value);
void shaderSetFloat(Shader* shader, const char* name, float value);
void shaderSetVec3(Shader* shader, const char* name, float x, float y, float z);
void shaderSetVec4(Shader* shader, const char* name, float x, float y, float z, float w);

#endif
