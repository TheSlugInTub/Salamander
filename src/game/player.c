#include <game/player.h>
#include <salamander/imgui_layer.h>
#include <salamander/state.h>
#include <salamander/input.h>

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
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player = SM_ECS_GET(smState.scene, _entity, Player);

        player->trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);
        player->rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);
        player->grounded = true;
    }
    SM_ECS_ITER_END();
}

bool DefaultBodyFilterFunc(void* context, JPH_BodyID bodyID)
{
    // This simple implementation always returns true,
    // meaning the ray will test against all bodies
    return true;
}

bool DefaultLockedBodyFilterFunc(void* context, const JPH_Body* bodyID)
{
    // This simple implementation always returns true,
    // meaning the ray will test against all bodies
    return true;
}

bool Player_IsGrounded(Player* player)
{
    JPH_Vec3 rayStart = {.x = player->trans->position[0],
                         .y = player->trans->position[1] - player->rigid->capsuleHeight + 0.2f,
                         .z = player->trans->position[2]};
    JPH_Vec3 rayEnd = {.x = player->trans->position[0],
                       .y = player->trans->position[1] - 3.0f,
                       .z = player->trans->position[2]};
    float    rayDistance = 5.0f;

    JPH_ObjectLayerFilter* objectLayerFilter =
        JPH_ObjectLayerFilter_Create(NULL);
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter =
        JPH_BroadPhaseLayerFilter_Create(NULL);

    JPH_RayCastResult           rayResult;
    const JPH_NarrowPhaseQuery* nQuery =
        JPH_PhysicsSystem_GetNarrowPhaseQuery(sm3d_state.system);

    JPH_BodyFilter* filter = JPH_BodyFilter_Create((void*)0);;
    JPH_BodyFilter_Procs procs;
    procs.ShouldCollide = DefaultBodyFilterFunc;
    procs.ShouldCollideLocked = DefaultLockedBodyFilterFunc;
    JPH_BodyFilter_SetProcs(&procs);

    bool rayHit = JPH_NarrowPhaseQuery_CastRay(
        nQuery, &rayStart, &rayEnd, &rayResult, broadPhaseLayerFilter,
        objectLayerFilter, filter);

    return rayHit;
}

void Player_ApplyMovement(Player* player, vec3 moveDirection)
{
    // Current velocity
    JPH_Vec3 currentVelocity = {0.0f, 0.0f, 0.0f};
    JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &currentVelocity);

    // Preserve vertical velocity for jumping/gravity
    JPH_Vec3 newVelocity = {moveDirection[0] * player->moveSpeed,
                            currentVelocity.y,
                            moveDirection[2] * player->moveSpeed};

    // Apply velocity
    JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &newVelocity);
}

// Jumping logic
void Player_Jump(Player* player)
{
    if (!player->grounded)
        return;

    JPH_Vec3 jumpVelocity = {0.0f, 0.0f, 0.0f};
    JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &jumpVelocity);

    // Add vertical jump velocity
    jumpVelocity.y = player->jumpSpeed;

    JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &jumpVelocity);
}

void Player_Sys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player = SM_ECS_GET(smState.scene, _entity, Player);

        vec3 cameraPos = {0.0f, 0.0f, 0.0f};
        glm_vec3_add(player->trans->position,
                     (vec3) {0.0f, 2.0f, 0.0f}, cameraPos);
        glm_vec3_copy(cameraPos, smState.camera.position);

        // Movement input handling
        vec3  moveDirection = {0.0f, 0.0f, 0.0f};
        float moveSpeed = player->moveSpeed;

        player->grounded = Player_IsGrounded(player);

        vec3 camFront;
        glm_vec3_copy(smState.camera.front, camFront);
        camFront[1] = 0.0f;
        glm_vec3_normalize(camFront);

        // Forward/Backward movement (W/S)
        if (smInput_GetKey(SM_KEY_W))
        {
            glm_vec3_mul(camFront,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDirection);
        }
        if (smInput_GetKey(SM_KEY_S))
        {
            glm_vec3_mul(camFront,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDirection);
            glm_vec3_inv(moveDirection);
        }

        // Strafe Left/Right (A/D)
        if (smInput_GetKey(SM_KEY_A))
        {
            vec3 moveDir = {0.0f, 0.0f, 0.0f};
            glm_vec3_mul(smState.camera.right,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDir);
            glm_vec3_inv(moveDir);
            glm_vec3_add(moveDir, moveDirection, moveDirection);
        }
        if (smInput_GetKey(SM_KEY_D))
        {
            vec3 moveDir = {0.0f, 0.0f, 0.0f};
            glm_vec3_mul(smState.camera.right,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDir);
            glm_vec3_add(moveDir, moveDirection, moveDirection);
        }

        // Apply movement
        Player_ApplyMovement(player, moveDirection);

        // Jumping (Space)
        if (smInput_GetKey(SM_KEY_SPACE))
            Player_Jump(player);
    }
    SM_ECS_ITER_END();
}
