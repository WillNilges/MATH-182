#ifndef CAMERA_H
#define CAMERA_H
#include "cglm/cglm.h"
#include <stdbool.h>
#include <stdlib.h>

float CAMERA_MAX_FOV = 90.0f;

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

typedef struct {
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float pitch;
    float yaw;
    float roll;

    // Configuration
    float speed;
    float fov; // Degrees
    float sensitivity;
} Camera;

Camera* newCameraWithDefaults();
Camera* newCamera(vec3 pos, vec3 front, vec3 up, float yaw, float pitch);
void cameraGetViewMatrix(Camera* camera, mat4 dest);
void cameraProcessKeyboard(Camera* camera, enum CameraMovement direction, float deltaTime);
void cameraProcessMouse(Camera* camera, float xOffset, float yOffset, bool constrainPitch);
void cameraProcessScroll(Camera* camera, float yOffset);
void cameraUpdateVectors(Camera* camera);
#endif
