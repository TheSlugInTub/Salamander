#include <salamander/state.h>

smEngineState smState = {};


void smSetState(smEngineState* state)
{
    smState = *state;
}
