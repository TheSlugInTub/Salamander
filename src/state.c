#include <salamander/state.h>

smEngineState smState = {};

void smSetState(smEngineState* state)
{
    smState = *state;
    glm_perspective(glm_rad(smState.camera.FOV),
                    smWindow_GetAspectRatio(smState.window), 0.1f,
                    1000.0f, smState.persProj);

    glm_ortho(0.0f, (float)smState.window->width, 0.0f,
              (float)smState.window->height, -100.0f, 100.0f,
              smState.orthoProj);
}
