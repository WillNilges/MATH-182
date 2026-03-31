#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <glad/glad.h>




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

Shader* newShader(char* vertexPath, char* fragmentPath)
{
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

void shaderSetVec3f(Shader* shader, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}

void shaderSetVec3(Shader* shader, const char* name, vec3 vec)
{
    glUniform3f(glGetUniformLocation(shader->ID, name), vec[0], vec[1], vec[2]);
}

void shaderSetVec4(Shader* shader, const char* name, vec4 vec)
{
    glUniform4f(glGetUniformLocation(shader->ID, name), vec[0], vec[1], vec[2], vec[3]);
}

void shaderSetMat4v(Shader* shader, const char* name, mat4 mat)
{
    glad_glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float*) mat);
}

// I hate this function.
char* shaderGetUniformName(char* uniformName, char* name, unsigned int index, char* property)
{
    size_t lenName = strlen(name);
    size_t lenProperty = strlen(property);
    size_t lenIndex = snprintf(NULL, 0, "%u", index);
    size_t lenExtra = 4; // Extra for '[].\0'

    size_t lenUniformName = lenName + lenProperty + lenIndex + lenExtra;

    snprintf(uniformName, lenUniformName, "%s[%u].%s", name, index, property);
    return uniformName;
}

void shader_loadDirLight(Shader* shader, DirLight* light, Camera* camera)
{
      if (light == NULL)
      {
        printf("Warning: Dir light is NULL. Bailing out");
        return;
      }

      // Do the directional lighting calculations
      // Process lighting information using provided camera
      vec3 viewspaceLightDir;
      glm_mat4_mulv3(camera->view, light->direction.raw, 1.0, viewspaceLightDir);
    
      shaderSetVec3(shader, "dirLight.direction", viewspaceLightDir);
      shaderSetVec3(shader, "dirLight.ambient", light->ambient.raw);
      shaderSetVec3(shader, "dirLight.diffuse", light->diffuse.raw);
      shaderSetVec3(shader, "dirLight.specular", light->specular.raw);
}


void shader_loadPointLight(Shader* shader, PointLight* light, int lightIdx, Camera* camera)
{
      // Convert light position to view space
      vec3 viewspaceLightPos;
      glm_mat4_mulv3(camera->view, light->position.raw, 1.0, viewspaceLightPos);

      char* uniformName = malloc(sizeof(char) * 64);
      shaderSetVec3 (shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "position"),  viewspaceLightPos);
      shaderSetFloat(shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "constant"),  light->constant);
      shaderSetFloat(shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "linear"),    light->linear);
      shaderSetFloat(shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "quadratic"), light->quadratic);
      shaderSetVec3 (shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "ambient"),   light->ambient.raw);
      shaderSetVec3 (shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "diffuse"),   light->diffuse.raw);
      shaderSetVec3 (shader,  shaderGetUniformName(uniformName, "pointLights", lightIdx, "specular"),  light->specular.raw);  

      free(uniformName);
}

void shader_loadSpotLight(Shader* shader, SpotLight* light, int lightIdx, Camera* camera)
{
    char* uniformName = malloc(sizeof(char) * 64);
    vec3 viewspaceLightPos;
    // Convert light position to world space
    glm_mat4_mulv3(camera->view, light->position.raw, 1.0, viewspaceLightPos);
    shaderSetVec3(shader,  shaderGetUniformName(uniformName, "spotLights", lightIdx, "position"), viewspaceLightPos);

    // Convert light direction to view space
    
    // FIXME: There's another conversion like this I need to do on the CPU.
    // Taking the inverse transpose of the model
    // before passing it to the vertex shader.
    vec3 viewspaceLightDir;
    mat3 Minv;
    // Extract 3×3 linear part (upper-left) of view matrix
    mat3 M;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            M[i][j] = camera->view[i][j];

    glm_mat3_inv(M, Minv);
    glm_mat3_transpose(Minv);  // in-place: Minv now = (M⁻¹)ᵀ
    glm_mat3_mulv(Minv, light->position.raw, viewspaceLightDir);
    glm_vec3_normalize(viewspaceLightDir);
           
    shaderSetVec3(shader,  shaderGetUniformName(uniformName, "spotLights", lightIdx, "direction"), viewspaceLightDir);

    shaderSetFloat(shader, shaderGetUniformName(uniformName, "spotLights", lightIdx, "cutOff"), light->cutOff);
    shaderSetFloat(shader, shaderGetUniformName(uniformName, "spotLights", lightIdx, "outerCutOff"), light->outerCutOff);
    shaderSetVec3(shader,  shaderGetUniformName(uniformName, "spotLights", lightIdx, "ambient"), light->ambient.raw);
    shaderSetVec3(shader,  shaderGetUniformName(uniformName, "spotLights", lightIdx, "diffuse"), light->diffuse.raw);
    shaderSetVec3(shader,  shaderGetUniformName(uniformName, "spotLights", lightIdx, "specular"), light->specular.raw);
    shaderSetFloat(shader, shaderGetUniformName(uniformName, "spotLights", lightIdx, "constant"), light->constant);
    shaderSetFloat(shader, shaderGetUniformName(uniformName, "spotLights", lightIdx, "linear"), light->linear);
    shaderSetFloat(shader, shaderGetUniformName(uniformName, "spotLights", lightIdx, "quadratic"), light->quadratic);

    free(uniformName);
}
