#include <salamander/components.h>
#include <salamander/state.h>
#include <salamander/renderer.h>
#include <salamander/systems.h>
#include <salamander/input.h>

void smSpriteRenderer_Sys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smSpriteRenderer))
    {
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);
        smSpriteRenderer* sprite =
            SM_ECS_GET(smState.scene, _entity, smSpriteRenderer);

        if (trans == NULL)
            continue;

        mat4 view;
        smCamera_GetViewMatrix(&smState.camera, view);

        smRenderer_RenderQuad(
            trans->position, trans->rotation[2],
            (vec2) {trans->scale[0], trans->scale[1]},
            sprite->texture, sprite->color, smState.persProj, view);
    }
    SM_ECS_ITER_END();
}

void smCamera_Sys()
{
    if (smInput_GetKey(SM_KEY_W))
    {
        smState.camera.position[1] += 0.05f;
    }
    if (smInput_GetKey(SM_KEY_S))
    {
        smState.camera.position[1] -= 0.05f;
    }
    if (smInput_GetKey(SM_KEY_A))
    {
        smState.camera.position[0] -= 0.05f;
    }
    if (smInput_GetKey(SM_KEY_D))
    {
        smState.camera.position[0] += 0.05f;
    }
    if (smInput_GetKey(SM_KEY_E))
    {
        smState.camera.position[2] -= 0.05f;
    }
    if (smInput_GetKey(SM_KEY_Q))
    {
        smState.camera.position[2] += 0.05f;
    }
}
