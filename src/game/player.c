#include <game/player.h>
#include <salamander/imgui_layer.h>
#include <salamander/state.h>

void Player_Draw(Player* player)
{
    if (smImGui_CollapsingHeader("Player"))
    {
        smImGui_DragFloat("MoveSpeed", &player->moveSpeed, 0.1f);
        smImGui_DragFloat("JumpSpeed", &player->jumpSpeed, 0.1f);
    }
}

smJson Player_Save(Player* player)
{
    smJson j = smJson_Create();

    smJson_SaveFloat(j, "MoveSpeed", player->moveSpeed);
    smJson_SaveFloat(j, "JumpSpeed", player->jumpSpeed);

    return j;
}

void Player_Load(Player* player, smJson j)
{
    smJson_LoadFloat(j, "MoveSpeed", &player->moveSpeed);
    smJson_LoadFloat(j, "JumpSpeed", &player->jumpSpeed);
}

void Player_StartSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player =
            SM_ECS_GET(smState.scene, _entity, Player);

        player->trans = SM_ECS_GET(smState.scene, _entity, smTransform);      
    }
    SM_ECS_ITER_END();
}

void Player_Sys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player =
            SM_ECS_GET(smState.scene, _entity, Player);

        glm_vec3_copy(player->trans->position, smState.camera.position);
    }
    SM_ECS_ITER_END();
}
