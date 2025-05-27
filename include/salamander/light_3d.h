#pragma once

#include <salamander/glm.h>
#include <salamander/vector.h>
#include <salamander/json_api.h>

#define SM_SHADOW_WIDTH  4096
#define SM_SHADOW_HEIGHT 4096
#define SM_MAX_LIGHTS    24

typedef struct
{
    vec3         position;
    vec3         direction;
    vec3         color;
    float        radius;
    float        intensity;
    float        falloff;
    bool         castsShadows;
    smVector*    shadowTransforms; // mat4
    unsigned int depthMapFBO;
    unsigned int depthCubemap;
    bool         directional;
} smLight3D;

void smLight3D_MakePointLight(smLight3D* light);

void smLight3D_StartSys();
void smLight3D_Sys();

void   smLight3D_Draw(smLight3D* light);
smJson smLight3D_Save(smLight3D* light);
void   smLight3D_Load(smLight3D* light, smJson j);
