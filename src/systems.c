#include <salamander/components.h>
#include <salamander/state.h>
#include <salamander/renderer.h>
#include <salamander/systems.h>
#include <salamander/input.h>

void SpriteRendererSys()
{
    ECS_ITER_START(smState.scene,
                   ECS_COMPONENT_TYPE(smSpriteRenderer))
    {
        smTransform* trans =
            ECS_GET(smState.scene, _entity, smTransform);
        smSpriteRenderer* sprite =
            ECS_GET(smState.scene, _entity, smSpriteRenderer);

        if (trans == NULL)
            continue;

        smQuad quad;
        glm_vec3_copy(trans->position, quad.position);
        quad.rotation = trans->rotation[2];
        quad.scale[0] = trans->scale[0];
        quad.scale[1] = trans->scale[1];
        quad.texture = sprite->texture;
        glm_vec4_copy(sprite->color, quad.color);
        glm_mat4_copy(smState.persProj, quad.projection);
        smCamera_GetViewMatrix(&smState.camera, quad.view);

        smRenderer_RenderQuad(&quad);
    }
    ECS_ITER_END();
}

void CameraSys()
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
