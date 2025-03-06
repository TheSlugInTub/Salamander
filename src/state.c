#include <salamander/state.h>

smEngineState smState = {};

void smSetState(smEngineState* state)
{
    smState = *state;
    glm_perspective(glm_rad(smState.camera.FOV),
                    smWindow_GetAspectRatio(smState.window), 0.1f,
                    100.0f, smState.persProj);
}
