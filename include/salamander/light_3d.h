#pragma once

#include <salamander/glm.h>
#include <salamander/vector.h>
#include <salamander/json_api.h>

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024

typedef struct
{
    vec3         position;
    vec3         color;
    float        radius;
    float        intensity;
    bool         castsShadows;
    smVector*    shadowTransforms; // mat4
    unsigned int depthMapFBO;
    unsigned int depthCubemap;
} smLight3D;

void smLight3D_MakePointLight(smLight3D* light);

void smLight3D_StartSys();
void smLight3D_Sys();

void   smLight3D_Draw(smLight3D* light);
smJson smLight3D_Save(smLight3D* light);
void   smLight3D_Load(smLight3D* light, smJson j);
