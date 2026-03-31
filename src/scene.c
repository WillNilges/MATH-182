#include "scene.h"
#include "cglm/mat3.h"
#include "entity.h"
#include <stdlib.h>

Scene* newScene()
{
  Scene* scene = malloc(sizeof(Scene));
  scene->shaders = malloc(sizeof(Shader));
  scene->lighting = malloc(sizeof(Lighting));
  scene->lighting->dirLight = NULL;
  scene->lighting->pointLights = NULL;
  scene->lighting->spotLights = NULL;
  scene->entities = malloc(sizeof(Entity));
  return scene;
}

void scene_registerDirLight(Scene* scene, DirLight* dirLight)
{
  scene->lighting->dirLight = dirLight;
}

// FIXME: Allow him to remove lights n shit
void scene_registerPointLight(Scene* scene, PointLight* pointLight)
{
  scene->lighting->pointLights = realloc(
    scene->lighting->pointLights,
    sizeof(PointLight) * (++scene->lighting->lenPointLights)
  );

  // FIXME: I feel like this is going to leak memory
  scene->lighting->pointLights[scene->lighting->lenPointLights - 1] = *pointLight;
}

void scene_registerSpotLight(Scene* scene, SpotLight* spotLight)
{
  scene->lighting->spotLights = realloc(
    scene->lighting->spotLights,
    sizeof(SpotLight) * (++scene->lighting->lenSpotLights)
  );

  // FIXME: I feel like this is going to leak memory
  scene->lighting->spotLights[scene->lighting->lenSpotLights - 1] = *spotLight;
}

void scene_registerShader(Scene* scene, Shader* shader)
{
  scene->shaders = realloc(
    scene->shaders,
    sizeof(Shader) * (++scene->lenShaders)
  );

  // FIXME: I feel like this is going to leak memory
  scene->shaders[scene->lenShaders - 1] = *shader;
}

void scene_registerEntity(Scene* scene, Entity* entity)
{
  scene->entities = realloc(
    scene->entities,
    sizeof(Entity) * (++scene->lenEntities)
  );

  // FIXME: I feel like this is going to leak memory
  scene->entities[scene->lenEntities - 1] = *entity;
}

void world_to_view(const vec3 worldPos, const vec3 worldNorm,
                   const mat4 view, vec3 outViewPos, vec3 outViewNorm) {
    // Extract 3×3 linear part (upper-left) of view matrix
    mat3 M;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            M[i][j] = view[i][j];

    // Position: transform with 3×3 (ignores translation)
    glm_mat3_mulv(M, worldPos, outViewPos);

    // Normal: transform with inverse transpose of M
    mat3 Minv;
    glm_mat3_inv(M, Minv);
    glm_mat3_transpose(Minv);  // in-place: Minv now = (M⁻¹)ᵀ
    glm_mat3_mulv(Minv, worldNorm,  outViewNorm);
    glm_vec3_normalize(outViewNorm);
}

void scene_draw(Scene* scene, Camera* camera)
{
  // For each shader
  // Build a list of entities that want to use this shader
  // activate the shader
  // set up the lights
  // draw the entities
  // Optimization: Only build that list once on startup and only change it when needed.

  for (int i = 0; i < scene->lenShaders; i++)
  {
    Shader* s = &(scene->shaders[i]);

    // Activate this shader
    shaderUse(s);

    // Pass the camera matricies in the shader
    shaderSetMat4v(s, "view", camera->view);
    shaderSetMat4v(s, "projection", camera->projection);

    if (scene->lighting->dirLight != NULL) {
      // Do the directional lighting calculations
      // Process lighting information using provided camera
      vec3 viewspaceLightDir;
      glm_mat4_mulv3(camera->view, scene->lighting->dirLight->direction.raw, 1.0, viewspaceLightDir);
    
      shaderSetVec3(s, "dirLight.direction", viewspaceLightDir);
      shaderSetVec3(s, "dirLight.ambient", scene->lighting->dirLight->ambient.raw);
      shaderSetVec3(s, "dirLight.diffuse", scene->lighting->dirLight->diffuse.raw);
      shaderSetVec3(s, "dirLight.specular", scene->lighting->dirLight->specular.raw);
    }

    // Set up point lights
    shaderSetInt(s, "pointLightCount", (int)(scene->lighting->lenPointLights));
    for (unsigned int j = 0; j < scene->lighting->lenPointLights; j++) {
      vec3 viewspaceLightPos;
      glm_mat4_mulv3(camera->view, scene->lighting->pointLights[j].position.raw, 1.0, viewspaceLightPos);
      shaderSetVec3(s,  shaderGetUniformName("pointLights", j, "position"),  viewspaceLightPos);
      shaderSetFloat(s, shaderGetUniformName("pointLights", j, "constant"),  scene->lighting->pointLights[j].constant);
      shaderSetFloat(s, shaderGetUniformName("pointLights", j, "linear"),    scene->lighting->pointLights[j].linear);
      shaderSetFloat(s, shaderGetUniformName("pointLights", j, "quadratic"), scene->lighting->pointLights[j].quadratic);
      shaderSetVec3(s,  shaderGetUniformName("pointLights", j, "ambient"),   scene->lighting->pointLights[j].ambient.raw);
      shaderSetVec3(s,  shaderGetUniformName("pointLights", j, "diffuse"),   scene->lighting->pointLights[j].diffuse.raw);
      shaderSetVec3(s,  shaderGetUniformName("pointLights", j, "specular"),  scene->lighting->pointLights[j].specular.raw);
    }

    // Set up spot lights
    // Add the flashlight info to the shader
    shaderSetInt(s, "spotLightCount", (int)(scene->lighting->lenSpotLights));
    for (unsigned int j = 0; j < scene->lighting->lenSpotLights; j++) {
        // FIXME: There is literally one piece I am missing here. Why does the spotlight move
        // when I move?
  
        // vec3 viewspaceLightPos;
        // glm_mat4_mulv3(camera->view, scene->lighting->spotLights[j].position.raw, 1.0, viewspaceLightPos);
        // shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "position"), viewspaceLightPos);

        vec3 viewspaceLightPos;
        vec3 viewspaceLightDir;
        
        world_to_view(
          scene->lighting->spotLights[j].position.raw,
          scene->lighting->spotLights[j].direction.raw,
          camera->view,
          viewspaceLightPos,
          viewspaceLightDir
        );

        //shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "position"), viewspaceLightPos);

        glm_mat4_mulv3(camera->view, scene->lighting->spotLights[j].position.raw, 1.0, viewspaceLightPos);
        shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "position"), viewspaceLightPos);

        
        shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "direction"), viewspaceLightDir);


        // shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "position"), scene->lighting->spotLights[j].position.raw);


        // vec3 viewspaceLightDir;
        // glm_mat4_mulv3(camera->view, scene->lighting->spotLights[j].direction.raw, 1.0, viewspaceLightDir);
        // shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "direction"), viewspaceLightDir);

        // glm_mat3_transpose(vec3 *m)

        // shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "direction"), scene->lighting->spotLights[j].direction.raw);

        shaderSetFloat(s, shaderGetUniformName("spotLights", j, "cutOff"), scene->lighting->spotLights[j].cutOff);
        shaderSetFloat(s, shaderGetUniformName("spotLights", j, "outerCutOff"), scene->lighting->spotLights[j].outerCutOff);
        shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "ambient"), scene->lighting->spotLights[j].ambient.raw);
        shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "diffuse"), scene->lighting->spotLights[j].diffuse.raw);
        shaderSetVec3(s,  shaderGetUniformName("spotLights", j, "specular"), scene->lighting->spotLights[j].specular.raw);
        shaderSetFloat(s, shaderGetUniformName("spotLights", j, "constant"), scene->lighting->spotLights[j].constant);
        shaderSetFloat(s, shaderGetUniformName("spotLights", j, "linear"), scene->lighting->spotLights[j].linear);
        shaderSetFloat(s, shaderGetUniformName("spotLights", j, "quadratic"), scene->lighting->spotLights[j].quadratic);
    }
    
    // XXX: Would it be nice to pass the lights into some kind of generic function per shader?

    for (int j = 0; j < scene->lenEntities; j++)
    {
      Entity* e = &(scene->entities[j]);
      if (e->shader->ID == s->ID)
      {
        entity_draw(e, camera);
      }
    }
  }
}
