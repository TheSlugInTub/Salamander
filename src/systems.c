#include <salamander/components.h>
#include <salamander/state.h>

void SpriteRendererSys()
{
    ECS_ITER_START(smState.scene,
                   ECS_COMPONENT_TYPE(smSpriteRenderer))
    {
        smTransform* trans =
            ECS_GET(smState.scene, _entity, smTransform);
        smSpriteRenderer* sprite =
            ECS_GET(smState.scene, _entity, smSpriteRenderer);


    }
    ECS_ITER_END();
}
