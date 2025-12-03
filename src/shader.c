#include "shader.h"
// Open and read the content of your shader files. Returns a char* containing
// the data from the shader.
// https://moderncprogramming.com/loading-a-glsl-shader-from-file-in-opengl-using-pure-c/
char* getShaderSourceFromFile(const char* filePath) {
    FILE* shaderFile = fopen (filePath, "rb");
    long size = 0;
    char* shaderContent;
    if (NULL == shaderFile) {
        fprintf(stderr, "Error: unable to open shader file '%s'\n", filePath);
        fflush (stderr);
        return(NULL);
    }

    printf("Reading shader source code from %s...\n", filePath); 

    // Check the length of the file
    fseek(shaderFile, 0L, SEEK_END);
    size = ftell(shaderFile)+1;
    fclose(shaderFile);

    // FIXME: is it possible to blow up the program by giving it a file that's too
    // big? Could we check the MIME types?

    // Now read the content of the file
    shaderFile = fopen(filePath, "r");
    shaderContent = memset(malloc(size), '\0', size);
    fread(shaderContent, 1, size-1, shaderFile);
    fclose(shaderFile);

    return shaderContent; // DON'T FORGET TO FREE THIS LATER
}

// Given paths to the shader programs, will compile and link a shader program
// and return its ID.
// A return value of 0 is an error, as it means something happened while compiling
// the shader.
// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateProgram.xhtml
unsigned int compileShaderProgram(char* vertexPath, char* fragmentPath) {
    // Set up our vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* vertexShaderSource = getShaderSourceFromFile(vertexPath);
    const GLchar* vertexShaderPtr = vertexShaderSource;
    glShaderSource(vertexShader, 1, &vertexShaderPtr, NULL);
    glCompileShader(vertexShader);

    // Check if compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR:SHADER:VERTEX:COMPILATION_FAILED\n%s\n", infoLog);
        glDeleteShader(vertexShader);
        free(vertexShaderSource);
        return 0;
    }

    // Set up our fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* fragmentShaderSource = getShaderSourceFromFile(fragmentPath);
    const GLchar* fragmentShaderPtr = fragmentShaderSource;
    glShaderSource(fragmentShader, 1, &fragmentShaderPtr, NULL);
    glCompileShader(fragmentShader);

    // Make sure the fragment shader compiled successfully
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR:SHADER:FRAGMENT:COMPILATION_FAILED\n%s\n", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        return 0;
    }

    // Time to create a shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check if the linking was successful
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR:SHADER:LINK_FAILED\n%s\n", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        return 0;
    }

    // Always clean up after ourselves

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(vertexShaderSource);
    free(fragmentShaderSource);

    return shaderProgram;
}

Shader* newShader(char* vertexPath, char* fragmentPath) {
    unsigned int shaderID = compileShaderProgram(vertexPath, fragmentPath);
    if (shaderID == 0) {
        printf("Shader compilation failed.\n");
        return NULL;
    }

    Shader* s = malloc(sizeof(Shader));
    s->ID = shaderID;
    s->vertexPath = vertexPath;
    s->fragmentPath = fragmentPath;

    printf("Returning shader with ID %d\n", s->ID);
    return s;
}

void shaderUse(Shader* shader) 
{
    glUseProgram(shader->ID);
}

void shaderSetInt(Shader* shader, const char* name, int value) 
{
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void shaderSetFloat(Shader* shader, const char* name, float value) 
{
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void shaderSetVec3(Shader* shader, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}

void shaderSetVec4(Shader* shader, const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}
