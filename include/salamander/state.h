#pragma once

#include <salamander/ecs_api.h>
#include <salamander/window.h>
#include <salamander/camera.h>

typedef struct
{
    smSceneHandle scene;
    smWindow*     window;
    smCamera      camera;
    mat4          persProj;
    mat4          orthoProj;
} smEngineState;

void smSetState(smEngineState* state);

extern smEngineState smState;
