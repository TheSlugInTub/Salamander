#pragma once

#include <salamander/ecs_api.h>
#include <salamander/glm.h>
#include <salamander/window.h>

typedef struct 
{
    SceneHandle scene;
    smWindow* window;
    mat4 persProj;
    mat4 orthoProj;
} smEngineState;

void smSetState(smEngineState* state);

extern smEngineState smState;