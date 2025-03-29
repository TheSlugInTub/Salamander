#include <game/player.h>
#include <salamander/imgui_layer.h>
#include <salamander/state.h>
#include <salamander/input.h>
#include <salamander/model.h>
#include <salamander/renderer.h>

void Player_Draw(Player* player)
{
    if (smImGui_CollapsingHeader("Player"))
    {
        smImGui_DragFloat("MoveSpeed", &player->moveSpeed, 0.1f);
        smImGui_DragFloat("JumpSpeed", &player->jumpSpeed, 0.1f);
        smImGui_DragFloat("LeafSpeed", &player->leafSpeed, 0.1f);
        smImGui_DragFloat("DashSpeed", &player->dashSpeed, 0.1f);
    }
}

smJson Player_Save(Player* player)
{
    smJson j = smJson_Create();

    smJson_SaveFloat(j, "MoveSpeed", player->moveSpeed);
    smJson_SaveFloat(j, "JumpSpeed", player->jumpSpeed);
    smJson_SaveFloat(j, "LeafSpeed", player->leafSpeed);
    smJson_SaveFloat(j, "DashSpeed", player->dashSpeed);

    return j;
}

void Player_Load(Player* player, smJson j)
{
    smJson_LoadFloat(j, "MoveSpeed", &player->moveSpeed);
    smJson_LoadFloat(j, "JumpSpeed", &player->jumpSpeed);
    smJson_LoadFloat(j, "LeafSpeed", &player->leafSpeed);
    smJson_LoadFloat(j, "DashSpeed", &player->dashSpeed);
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

bool DefaultLockedBodyFilterFunc(void*           context,
                                 const JPH_Body* bodyID)
{
    // This simple implementation always returns true,
    // meaning the ray will test against all bodies
    return true;
}

bool RaycastBodyFilterFunc(void* context, JPH_BodyID bodyID)
{
    JPH_ObjectLayer targetLayer = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterface, bodyID);

    // Only allow rays to hit the specific layer
    return sm3d_Layers_LEAF == targetLayer;
}

void Player_LeafDash(Player* player)
{
    JPH_Vec3 rayStart = {.x = smState.camera.position[0],
                         .y = smState.camera.position[1],
                         .z = smState.camera.position[2]};

    vec3 camFront;
    glm_vec3_copy(smState.camera.front, camFront);
    glm_vec3_mul(camFront, (vec3) {100.0f, 100.0f, 100.0f}, camFront);

    vec3 rayEndGLM;
    glm_vec3_add(smState.camera.position, camFront, rayEndGLM);

    JPH_Vec3 rayEnd = {.x = rayEndGLM[0],
                       .y = rayEndGLM[1],
                       .z = rayEndGLM[2]};

    JPH_ObjectLayerFilter* objectLayerFilter =
        JPH_ObjectLayerFilter_Create(NULL);
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter =
        JPH_BroadPhaseLayerFilter_Create(NULL);

    JPH_RayCastResult           rayResult;
    const JPH_NarrowPhaseQuery* nQuery =
        JPH_PhysicsSystem_GetNarrowPhaseQuery(sm3d_state.system);

    JPH_BodyFilter* filter = JPH_BodyFilter_Create(NULL);

    JPH_BodyFilter_Procs procs;
    procs.ShouldCollide = RaycastBodyFilterFunc;
    procs.ShouldCollideLocked = DefaultLockedBodyFilterFunc;
    JPH_BodyFilter_SetProcs(&procs);

    bool rayHit = JPH_NarrowPhaseQuery_CastRay(
        nQuery, &rayStart, &rayEnd, &rayResult, broadPhaseLayerFilter,
        objectLayerFilter, filter);

    if (rayHit)
    {
        printf("rayhit!\n");

        JPH_Vec3 jumpVelocity = {0.0f, 0.0f, 0.0f};
        JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &jumpVelocity);

        jumpVelocity.x = smState.camera.front[0] * player->dashSpeed;
        jumpVelocity.y = smState.camera.front[1] * player->dashSpeed;
        jumpVelocity.z = smState.camera.front[2] * player->dashSpeed;

        JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &jumpVelocity);
    }
}

bool Player_IsGrounded(Player* player)
{
    JPH_Vec3 rayStart = {.x = player->trans->position[0],
                         .y = player->trans->position[1] -
                              player->rigid->capsuleHeight + 0.2f,
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

    JPH_BodyFilter* filter = JPH_BodyFilter_Create((void*)0);

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
    JPH_Vec3 newVelocity = {
        currentVelocity.x + moveDirection[0] * player->moveSpeed,
        currentVelocity.y,
        currentVelocity.z + moveDirection[2] * player->moveSpeed};

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

        JPH_Vec3 curVelocity = {0.0f, 0.0f, 0.0f};
        JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &curVelocity);

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
            vec3 moveDir = {0.0f, 0.0f, 0.0f};
            glm_vec3_mul(camFront,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDir);
            glm_vec3_add(moveDir, moveDirection, moveDirection);
        }
        if (smInput_GetKey(SM_KEY_S))
        {
            vec3 moveDir = {0.0f, 0.0f, 0.0f};
            glm_vec3_mul(camFront,
                         (vec3) {moveSpeed, moveSpeed, moveSpeed},
                         moveDir);
            glm_vec3_add(moveDir, moveDirection, moveDirection);
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

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_LEFT))
        {
            Player_LeafDash(player);
        }

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_RIGHT))
        {
            smEntityID leafEnt = smECS_AddEntity(smState.scene);

            smTransform* leafTrans =
                SM_ECS_ASSIGN(smState.scene, leafEnt, smTransform);
            glm_vec3_copy(smState.camera.position,
                          leafTrans->position);
            glm_vec3_copy((vec3) {3.0f, 3.0f, 3.0f},
                          leafTrans->scale);

            vec3 cameraDir;
            vec3 leafPos;
            vec3 camFront;
            glm_vec3_mul(smState.camera.front,
                         (vec3) {3.0f, 3.0f, 3.0f}, camFront);
            glm_vec3_add(leafTrans->position, camFront, leafPos);

            vec3 direction;
            glm_vec3_sub(smState.camera.position, leafPos, direction);
            glm_vec3_normalize(direction);

            smRigidbody3D* leafBody =
                SM_ECS_ASSIGN(smState.scene, leafEnt, smRigidbody3D);
            leafBody->bodyType = sm3d_Leaf;
            leafBody->colliderType = sm3d_Sphere;
            leafBody->sphereRadius = 1.0f;
            leafBody->mass = 1.0f;
            leafBody->friction = 0.1f;
            leafBody->linearDamping = 0.1f;
            leafBody->angularDamping = 0.1f;
            leafBody->restitution = 0.1f;
            leafBody->fixedRotation = false;

            smMeshRenderer* leafMesh =
                SM_ECS_ASSIGN(smState.scene, leafEnt, smMeshRenderer);
            strcpy(leafMesh->modelPath, "res/models/leaf.obj");
            strcpy(leafMesh->texturePath, "res/textures/leaf.png");
            leafMesh->gammaCorrection = false;
            leafMesh->extractTexture = false;

            smModel_Create(leafMesh);

            smModel_Load(leafMesh, leafMesh->modelPath);

            smName* leafName =
                SM_ECS_ASSIGN(smState.scene, leafEnt, smName);
            strcpy(leafName->name, "Leaf");

            smPhysics3D_CreateBody(leafBody, leafTrans);

            JPH_Vec3 vel = {
                smState.camera.front[0] * player->leafSpeed,
                smState.camera.front[1] * player->leafSpeed,
                smState.camera.front[2] * player->leafSpeed};

            JPH_BodyInterface_SetLinearVelocity(
                sm3d_state.bodyInterface, leafBody->bodyID, &vel);

            JPH_Vec3 quatRot = {direction[0], direction[1],
                                direction[2]};
            JPH_Quat quaternionRotation;

            JPH_Quat_FromEulerAngles(&quatRot, &quaternionRotation);

            // Set the rotation for the body
            JPH_BodyInterface_SetRotation(
                sm3d_state
                    .bodyInterface, // Your body interface pointer
                leafBody->bodyID,   // The body ID you want to rotate
                &quaternionRotation, JPH_Activation_Activate);
        }
    }
    SM_ECS_ITER_END();
}
