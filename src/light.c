#include "light.h"
#include "camera.h"
#include "cglm/mat4.h"
#include "scene.h"
#include <stdio.h>

void dirLight_setDirection(DirLight* light, float x, float y, float z) {
  light->direction.x = x;
  light->direction.y = y;
  light->direction.z = z;
}

void dirLight_setAmbient(DirLight* light, float x, float y, float z) {
  light->ambient.x = x;
  light->ambient.y = y;
  light->ambient.z = z;
}

void dirLight_setDiffuse(DirLight* light, float x, float y, float z) {
  light->diffuse.x = x;
  light->diffuse.y = y;
  light->diffuse.z = z;
}

void dirLight_setSpecular(DirLight* light, float x, float y, float z) {
  light->specular.x = x;
  light->specular.y = y;
  light->specular.z = z;
}

// FIXME: Can I get rid of the camera?
void dirLight_setInShader(DirLight* light, Camera* camera, Shader *shader)
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

void pointLight_setInShader(PointLight* light, Camera* camera, Shader* shader, int lightIdx)
{
      // Convert light position to view space
      vec3 viewspaceLightPos;
      glm_mat4_mulv3(camera->view, light->position.raw, 1.0, viewspaceLightPos);

      shaderSetVec3 (shader,  shaderGetUniformName("pointLights", lightIdx, "position"),  viewspaceLightPos);
      shaderSetFloat(shader,  shaderGetUniformName("pointLights", lightIdx, "constant"),  light->constant);
      shaderSetFloat(shader,  shaderGetUniformName("pointLights", lightIdx, "linear"),    light->linear);
      shaderSetFloat(shader,  shaderGetUniformName("pointLights", lightIdx, "quadratic"), light->quadratic);
      shaderSetVec3 (shader,  shaderGetUniformName("pointLights", lightIdx, "ambient"),   light->ambient.raw);
      shaderSetVec3 (shader,  shaderGetUniformName("pointLights", lightIdx, "diffuse"),   light->diffuse.raw);
      shaderSetVec3 (shader,  shaderGetUniformName("pointLights", lightIdx, "specular"),  light->specular.raw);  
}

void spotLight_setInShader(SpotLight* light, Camera* camera, Shader* shader, int lightIdx)
{
    vec3 viewspaceLightPos;
    // Convert light position to world space
    glm_mat4_mulv3(camera->view, light->position.raw, 1.0, viewspaceLightPos);
    shaderSetVec3(shader,  shaderGetUniformName("spotLights", lightIdx, "position"), viewspaceLightPos);

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
           
    shaderSetVec3(shader,  shaderGetUniformName("spotLights", lightIdx, "direction"), viewspaceLightDir);

    shaderSetFloat(shader, shaderGetUniformName("spotLights", lightIdx, "cutOff"), light->cutOff);
    shaderSetFloat(shader, shaderGetUniformName("spotLights", lightIdx, "outerCutOff"), light->outerCutOff);
    shaderSetVec3(shader,  shaderGetUniformName("spotLights", lightIdx, "ambient"), light->ambient.raw);
    shaderSetVec3(shader,  shaderGetUniformName("spotLights", lightIdx, "diffuse"), light->diffuse.raw);
    shaderSetVec3(shader,  shaderGetUniformName("spotLights", lightIdx, "specular"), light->specular.raw);
    shaderSetFloat(shader, shaderGetUniformName("spotLights", lightIdx, "constant"), light->constant);
    shaderSetFloat(shader, shaderGetUniformName("spotLights", lightIdx, "linear"), light->linear);
    shaderSetFloat(shader, shaderGetUniformName("spotLights", lightIdx, "quadratic"), light->quadratic);
}
