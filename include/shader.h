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
