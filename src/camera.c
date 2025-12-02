#include "camera.h"
#include "cglm/io.h"
#include "cglm/vec3.h"

float CAMERA_MAX_FOV = 90.0f;
float CAMERA_SPEED = 1.0f;

Camera* newCameraWithDefaults()
{
    vec3 pos = { 0.0f, 0.0f, 3.0f };
    vec3 front = { 0.0f, 0.0f, -1.0f };
    vec3 up = { 0.0f, 1.0f, 0.0f };

    float pitch = 0.0f;
    float yaw = -90.0f;

    float fov = 90.0f;
    float sensitivity = 0.1f;

    Camera* cam = newCamera(pos, front, up, yaw, pitch);

    cam->fov = fov;
    cam->sensitivity = sensitivity;
    cam->speed = CAMERA_SPEED;

    return cam;
}

Camera* newCamera(vec3 pos, vec3 front, vec3 up, float yaw, float pitch)
{
    Camera* camera = malloc(sizeof(Camera));
    glm_vec3_copy(pos, camera->pos);
    glm_vec3_copy(front, camera->front);
    glm_vec3_copy(up, camera->up);
    glm_vec3_copy(up, camera->worldUp);

    camera->yaw = yaw;
    camera->pitch = pitch;

    return camera;
}

void cameraGetViewMatrix(Camera* camera, mat4 view)
{
    glm_mat4_identity(view);
    vec3 center;
    glm_vec3_add(camera->pos, camera->front, center);
    glm_lookat(camera->pos, center, camera->up, view);
}

void cameraProcessKeyboard(Camera* camera, enum CameraMovement direction, float deltaTime)
{
    vec3 cameraSpeedXCameraFront;
    vec3 cameraRight;
    vec3 upVec;
    float velocity = camera->speed * deltaTime;

    switch (direction) {
        case FORWARD:
            glm_vec3_scale(camera->front, velocity, cameraSpeedXCameraFront);
            //cameraSpeedXCameraFront[1] = 0.0f; // No vertical movement
            glm_vec3_add(camera->pos, cameraSpeedXCameraFront, camera->pos);
            break;
        case BACKWARD:
            glm_vec3_scale(camera->front, velocity, cameraSpeedXCameraFront);
            //cameraSpeedXCameraFront[1] = 0.0f; // No vertical movement
            glm_vec3_sub(camera->pos, cameraSpeedXCameraFront, camera->pos);
            break;
        case LEFT:
            glm_cross(camera->front, camera->up, cameraRight);
            glm_normalize(cameraRight);
            glm_vec3_scale(cameraRight, velocity, cameraRight);
            //cameraRight[1] = 0.0f; // No vertical movement
            glm_vec3_sub(camera->pos, cameraRight, camera->pos);
            break;
        case RIGHT:
            glm_cross(camera->front, camera->up, cameraRight);
            glm_normalize(cameraRight);
            glm_vec3_scale(cameraRight, velocity, cameraRight);
            //cameraRight[1] = 0.0f; // No vertical movement
            glm_vec3_add(camera->pos, cameraRight, camera->pos);
            break;
        case UP:
            glm_vec3_scale(camera->worldUp, velocity, upVec);
            glm_vec3_add(camera->pos, upVec, camera->pos);
        default:
            break;
    }

    cameraUpdateVectors(camera);
}

void cameraProcessMouse(Camera* camera, float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= camera->sensitivity;
    yOffset *= camera->sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if (camera->pitch > 89.0f)
    {
        camera->pitch = 89.0f;
    }

    if (camera->pitch < -89.0f)
    {
        camera->pitch = -89.0f;
    }

    vec3 direction = {
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch))
    };

    glm_normalize(direction);
    glm_vec3_copy(direction, camera->front);

    cameraUpdateVectors(camera);
}

void cameraProcessScroll(Camera* camera, float yOffset)
{
    camera->fov -= (float)yOffset;
    if (camera->fov < 1.0f)
    {
        camera->fov = 1.0f;
    }
    if (camera->fov > CAMERA_MAX_FOV)
    {
        camera->fov = CAMERA_MAX_FOV; 
    }
}

void cameraUpdateVectors(Camera* camera)
{
    // Re-compute the front of the camera
    camera->front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    camera->front[1] = sin(glm_rad(camera->pitch));
    camera->front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_normalize(camera->front);

    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm_cross(camera->front, camera->worldUp, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->up);
}
