#pragma once

#include <salamander/glm.h>

typedef struct
{
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;

    float yaw;
    float pitch;
    float zoom;
    float FOV;

    vec3 worldUp;
} smCamera;

// Initializes the camera
smCamera smCamera_Create(vec3 position, vec3 up,
                     float yaw, float pitch, float FOV);

// Updates the right, up and front vectors 
void smCamera_UpdateVectors(smCamera* camera);

// Gets the view matrix of the camera
void smCamera_GetViewMatrix(smCamera* camera, mat4 view);
