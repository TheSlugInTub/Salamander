#include <game/player.h>
#include <salamander/imgui_layer.h>
#include <salamander/state.h>
#include <salamander/input.h>
#include <salamander/model.h>
#include <salamander/renderer.h>
#include <salamander/config.h>
#include <salamander/utils.h>

PlayerData playerData;

void Player_Draw(Player* player)
{
    if (smImGui_CollapsingHeader("Player"))
    {
        smImGui_DragFloat("Speed", &player->speed, 0.1f);
        smImGui_DragFloat("SlideSpeed", &player->slideSpeed, 0.1f);
        smImGui_DragFloat("Acceleration", &player->acceleration,
                          0.1f);
        smImGui_DragFloat("Deceleration", &player->deceleration,
                          0.1f);
        smImGui_DragFloat("AirSpeed", &player->airSpeed, 0.1f);
        smImGui_DragFloat("AirAcceleration", &player->airAcceleration,
                          0.1f);
        smImGui_DragFloat("JumpSpeed", &player->jumpSpeed, 0.1f);
        smImGui_DragFloat("DashSpeed", &player->dashSpeed, 0.1f);
        smImGui_DragFloat("LeafRegenSpeed", &player->leafRegenSpeed,
                          0.1f);
        smImGui_DragFloat2("LeafSpriteScale", player->leafSpriteScale,
                           0.1f);

        smImGui_Checkbox("IsGrounded", &player->grounded);
    }
}

smJson Player_Save(Player* player)
{
    smJson j = smJson_Create();

    smJson_SaveFloat(j, "Speed", player->speed);
    smJson_SaveFloat(j, "SlideSpeed", player->slideSpeed);
    smJson_SaveFloat(j, "Acceleration", player->acceleration);
    smJson_SaveFloat(j, "Deceleration", player->deceleration);
    smJson_SaveFloat(j, "AirSpeed", player->airSpeed);
    smJson_SaveFloat(j, "AirAcceleration", player->airAcceleration);
    smJson_SaveFloat(j, "JumpSpeed", player->jumpSpeed);
    smJson_SaveFloat(j, "DashSpeed", player->dashSpeed);
    smJson_SaveFloat(j, "LeafRegenSpeed", player->leafRegenSpeed);
    smJson_SaveVec2(j, "LeafSpriteScale", player->leafSpriteScale);

    return j;
}

void Player_Load(Player* player, smJson j)
{
    smJson_LoadFloat(j, "Speed", &player->speed);
    smJson_LoadFloat(j, "SlideSpeed", &player->slideSpeed);
    smJson_LoadFloat(j, "Acceleration", &player->acceleration);
    smJson_LoadFloat(j, "Deceleration", &player->deceleration);
    smJson_LoadFloat(j, "AirSpeed", &player->airSpeed);
    smJson_LoadFloat(j, "AirAcceleration", &player->airAcceleration);
    smJson_LoadFloat(j, "JumpSpeed", &player->jumpSpeed);
    smJson_LoadFloat(j, "DashSpeed", &player->dashSpeed);
    smJson_LoadFloat(j, "LeafRegenSpeed", &player->leafRegenSpeed);
    smJson_LoadVec2(j, "LeafSpriteScale", player->leafSpriteScale);
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
        player->state = PlayerState_Flying;

        glm_vec3_copy((vec3) {0.0f, 1.0f, 0.0f},
                      player->groundNormal);

        playerData.leafCount = 3;
        player->currentLeafCount = playerData.leafCount;

        player->fullLeafSprite =
            smUtils_LoadTexture("res/textures/LeafSprite.png");
        player->emptyLeafSprite =
            smUtils_LoadTexture("res/textures/EmptyLeafSprite.png");

        vec2 lastLeafPos = {40.0f, 40.0f};

        for (int i = 0; i < player->currentLeafCount; ++i)
        {
            smEntityID ent = smECS_AddEntity(smState.scene);
            smName* name = SM_ECS_ASSIGN(smState.scene, ent, smName);
            strcpy(name->name, "LeafImage");

            player->leafImages[i] =
                SM_ECS_ASSIGN(smState.scene, ent, smImage);
            strcpy(player->leafImages[i]->texturePath,
                   "res/textures/LeafSprite.png");
            player->leafImages[i]->texture = smUtils_LoadTexture(
                player->leafImages[i]->texturePath);

            glm_vec4_copy((vec4) {1.0f, 1.0f, 1.0f, 1.0f},
                          player->leafImages[i]->color);

            glm_vec2_copy(player->leafSpriteScale,
                          player->leafImages[i]->scale);

            glm_vec2_copy(lastLeafPos,
                          player->leafImages[i]->position);

            glm_vec2_add(lastLeafPos,
                         (vec2) {player->leafSpriteScale[0], 0.0f},
                         lastLeafPos);
            glm_vec2_add(lastLeafPos, (vec2) {25.0f, 0.0f},
                         lastLeafPos);
        }
    }
    SM_ECS_ITER_END();
}

bool DefaultBodyFilterFunc(void* context, JPH_BodyID bodyID)
{
    JPH_ObjectLayer targetLayer = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterface, bodyID);

    return targetLayer == sm3d_Layers_PLAYER ? false : true;
}

bool DefaultLockedBodyFilterFunc(void*           context,
                                 const JPH_Body* bodyID)
{
    // This simple implementation always returns true,
    // meaning the ray will test against all bodies
    return true;
}

JPH_Vec3 leafPos;

bool RaycastBodyFilterFunc(void* context, JPH_BodyID bodyID)
{
    JPH_ObjectLayer targetLayer = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterface, bodyID);

    JPH_BodyInterface_GetPosition(sm3d_state.bodyInterface, bodyID,
                                  &leafPos);

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
    glm_vec3_mul(camFront, (vec3) {10000.0f, 10000.0f, 10000.0f},
                 camFront);

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
        // Calculate distance between player and hit point
        JPH_Vec3 playerPosition;
        JPH_BodyInterface_GetPosition(sm3d_state.bodyInterface,
                                      player->rigid->bodyID,
                                      &playerPosition);
        // Calculate distance vector
        JPH_Vec3 distanceVec = {leafPos.x - playerPosition.x,
                                leafPos.y - playerPosition.y,
                                leafPos.z - playerPosition.z};

        float distance = JPH_Vec3_Length(&distanceVec);

        // Calculate scaled dash speed based on distance
        float scaledDashSpeed = player->dashSpeed * distance;

        // Apply the scaled dash speed in the camera's direction
        JPH_Vec3 dashForce = {0.0f, 0.0f, 0.0f};

        dashForce.x = smState.camera.front[0] * scaledDashSpeed;
        dashForce.y = smState.camera.front[1] * scaledDashSpeed;
        dashForce.z = smState.camera.front[2] * scaledDashSpeed;

        JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                                   player->rigid->bodyID, &dashForce);
    }
}

bool Player_IsGrounded(Player* player)
{
    JPH_Vec3 rayStart = {.x = player->trans->position[0],
                         .y = player->trans->position[1] -
                              player->rigid->capsuleHeight + 0.4f,
                         .z = player->trans->position[2]};
    JPH_Vec3 rayEnd = {.x = player->trans->position[0],
                       .y = player->trans->position[1] - 1.0f,
                       .z = player->trans->position[2]};

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

void Player_Walk(Player* player, vec3 direction, float acceleration)
{
    if (smInput_GetKey(SM_KEY_SPACE))
    {
        Player_Jump(player);
        return;
    }

    vec3 wishDir;
    glm_vec3_copy(direction, wishDir);
    wishDir[1] = 0.0f;

    float directionMagnitude = glm_vec3_norm(direction);
    float wishDirMagnitude = glm_vec3_norm(wishDir);

    // Deceleration when standing still
    if (directionMagnitude < 0.1f && !player->crouched)
    {
        JPH_Vec3 curVelocity = {0.0f, 0.0f, 0.0f};
        JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &curVelocity);

        // Apply gradual deceleration on horizontal plane
        vec3 decelerationForce = {
            -curVelocity.x * player->deceleration, 0.0f,
            -curVelocity.z * player->deceleration};

        // Only apply deceleration if we're actually moving
        if (fabs(curVelocity.x) > 0.1f || fabs(curVelocity.z) > 0.1f)
        {
            JPH_Vec3 jphDecel = {decelerationForce[0],
                                 decelerationForce[1],
                                 decelerationForce[2]};
            JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                                       player->rigid->bodyID,
                                       &jphDecel);
        }
        return;
    }
    // Deceleration when moving
    else if (!player->crouched)
    {
        JPH_Vec3 curVelocity = {0.0f, 0.0f, 0.0f};
        JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &curVelocity);

        // Apply gradual deceleration on horizontal plane
        vec3 decelerationForce = {
            -curVelocity.x * player->deceleration * 0.4f, 0.0f,
            -curVelocity.z * player->deceleration * 0.4f};

        // Only apply deceleration if we're actually moving
        if (fabs(curVelocity.x) > 0.1f || fabs(curVelocity.z) > 0.1f)
        {
            JPH_Vec3 jphDecel = {decelerationForce[0],
                                 decelerationForce[1],
                                 decelerationForce[2]};
            JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                                       player->rigid->bodyID,
                                       &jphDecel);
        }
    }

    float playerSpeed =
        player->crouched == true ? player->slideSpeed : player->speed;

    if (glm_vec3_norm(wishDir) > playerSpeed)
    {
        acceleration *= wishDirMagnitude / playerSpeed;
    }

    vec3 moveDir = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(wishDir, playerSpeed, moveDir);
    glm_vec3_sub(moveDir, wishDir, moveDir);

    if (directionMagnitude < 0.5f)
    {
        acceleration *= directionMagnitude / 0.5f;
    }

    glm_normalize(moveDir);
    glm_vec3_muladds(moveDir, acceleration, moveDir);
    glm_vec3_muladds(moveDir, directionMagnitude, moveDir);

    vec3 slopeCorrection = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(player->groundNormal, -9.81f, slopeCorrection);
    glm_vec3_divs(slopeCorrection, player->groundNormal[1],
                  slopeCorrection);

    glm_vec3_add(moveDir, slopeCorrection, moveDir);

    JPH_Vec3 jphMoveDir = {moveDir[0], moveDir[1], moveDir[2]};

    JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                               player->rigid->bodyID, &jphMoveDir);
}

float smMax(float a, float b)
{
    return a > b ? a : b;
}

float smMin(float a, float b)
{
    return a > b ? b : a;
}

void Player_Fly(Player* player, vec3 direction)
{
    JPH_Vec3 curVelocity = {0.0f, 0.0f, 0.0f};
    JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &curVelocity);

    vec3 curVel = {curVelocity.x, 0.0f, curVelocity.z};

    float projVel = glm_vec3_dot(curVel, direction);

    float accelVel = player->airAcceleration * smState.deltaTime;

    if (projVel + accelVel > player->airSpeed)
    {
        accelVel = smMax(0.0f, player->airSpeed - projVel);
    }

    vec3 directionNormalized;
    glm_vec3_copy(direction, directionNormalized);
    glm_vec3_normalize(directionNormalized);

    // Calculate acceleration vector
    vec3 accelVector = {0.0f, 0.0f, 0.0f};
    glm_vec3_scale(directionNormalized, accelVel, accelVector);

    // Add acceleration to current velocity
    JPH_Vec3 newVelocity = {curVelocity.x + accelVector[0],
                            curVelocity.y,
                            curVelocity.z + accelVector[2]};

    JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &newVelocity);
}

void Player_Sys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player = SM_ECS_GET(smState.scene, _entity, Player);

        JPH_Vec3 curVelocity = {0.0f, 0.0f, 0.0f};
        JPH_BodyInterface_GetLinearVelocity(sm3d_state.bodyInterface,
                                            player->rigid->bodyID,
                                            &curVelocity);

        player->grounded = Player_IsGrounded(player);

        vec3 camFront;
        glm_vec3_copy(smState.camera.front, camFront);
        camFront[1] = 0.0f;
        glm_vec3_normalize(camFront);

        // Movement input handling
        vec3 moveDirection = {0.0f, 0.0f, 0.0f};

        // Movement keys
        if (smInput_GetKey(SM_KEY_W))
        {
            glm_vec3_add(camFront, moveDirection, moveDirection);
        }
        if (smInput_GetKey(SM_KEY_S))
        {
            glm_vec3_add(camFront, moveDirection, moveDirection);
            glm_vec3_inv(moveDirection);
        }
        if (smInput_GetKey(SM_KEY_A))
        {
            vec3 invCamRight;
            glm_vec3_copy(smState.camera.right, invCamRight);
            glm_vec3_inv(invCamRight);
            glm_vec3_add(invCamRight, moveDirection, moveDirection);
        }
        if (smInput_GetKey(SM_KEY_D))
        {
            glm_vec3_add(smState.camera.right, moveDirection,
                         moveDirection);
        }

        glm_normalize(moveDirection);

        if (smInput_GetKey(SM_KEY_LEFT_CONTROL))
        {
            player->crouched = true;

            vec3 cameraPos = {0.0f, 0.0f, 0.0f};
            glm_vec3_add(player->trans->position,
                         (vec3) {0.0f, 1.0f, 0.0f}, cameraPos);
            glm_vec3_copy(cameraPos, smState.camera.position);
            smState.camera.roll = -0.3f;
        }
        else
        {
            player->crouched = false;

            vec3 cameraPos = {0.0f, 0.0f, 0.0f};
            glm_vec3_add(player->trans->position,
                         (vec3) {0.0f, 2.0f, 0.0f}, cameraPos);
            glm_vec3_copy(cameraPos, smState.camera.position);
            smState.camera.roll = 0.0f;
        }

        switch (player->state)
        {
            case PlayerState_Walking:
            {
                Player_Walk(player, moveDirection,
                            player->acceleration);
                break;
            }
            case PlayerState_Flying:
            {
                Player_Fly(player, moveDirection);
                break;
            }
            case PlayerState_Wallrunning:
            {
                break;
            }
        }

        // Movement
        if (player->grounded)
        {
            player->state = PlayerState_Walking;
        }
        else
        {
            player->state = PlayerState_Flying;
        }

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_LEFT))
        {
            Player_LeafDash(player);
        }

        player->leafRegenTimer -= smState.deltaTime;

        if (player->leafRegenTimer <= 0.0f &&
            player->currentLeafCount < playerData.leafCount)
        {
            player->leafImages[player->currentLeafCount]->texture =
                player->fullLeafSprite;

            player->leafRegenTimer = player->leafRegenSpeed;
            player->currentLeafCount++;
        }

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_RIGHT) &&
            player->currentLeafCount > 0)
        {
            smEntityID leafEnt = smECS_AddEntity(smState.scene);

            player->currentLeafCount--;
            player->leafRegenTimer = player->leafRegenSpeed;
            player->leafImages[player->currentLeafCount]->texture =
                player->emptyLeafSprite;

            smTransform* leafTrans =
                SM_ECS_ASSIGN(smState.scene, leafEnt, smTransform);
            glm_vec3_copy(smState.camera.position,
                          leafTrans->position);
            glm_vec3_copy((vec3) {3.0f, 3.0f, 3.0f},
                          leafTrans->scale);

            vec3 leafPos;
            vec3 camFront;
            glm_vec3_mul(smState.camera.front,
                         (vec3) {3.0f, 3.0f, 3.0f}, camFront);

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

            JPH_Vec3 vel = {smState.camera.front[0] * 20.0f,
                            smState.camera.front[1] * 20.0f,
                            smState.camera.front[2] * 20.0f};
            JPH_Vec3_Add(&curVelocity, &vel, &vel);

            JPH_BodyInterface_SetLinearVelocity(
                sm3d_state.bodyInterface, leafBody->bodyID, &vel);

            vec3 direction;
            glm_vec3_sub(smState.camera.position, leafPos, direction);
            glm_vec3_add(
                (vec3) {curVelocity.x, curVelocity.y, curVelocity.z},
                direction, direction);
            glm_vec3_normalize(direction);

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
