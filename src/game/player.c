#include <game/player.h>
#include <salamander/imgui_layer.h>
#include <salamander/input.h>
#include <salamander/model.h>
#include <salamander/renderer.h>
#include <salamander/config.h>
#include <salamander/utils.h>
#include <salamander/editor.h>

Player* globalPlayer;
PlayerData playerData;

JPH_ContactListener*      playerContactListener;
JPH_ContactListener_Procs playerContactListenerProcs;

JPH_Vec3 hurtDir;

void Player_Hurt(Player* player, int damage);

JPH_ValidateResult Player_EnemyContactValidate(
    void* userData, const JPH_Body* body1, const JPH_Body* body2,
    const JPH_RVec3* baseOffset, const JPH_CollideShapeResult* result)
{
    JPH_BodyID id1 = JPH_Body_GetID(body1);
    JPH_BodyID id2 = JPH_Body_GetID(body2);

    JPH_ObjectLayer layer1 = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterfaceNoLock, id1);
    JPH_ObjectLayer layer2 = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterfaceNoLock, id2);
    
    // Check if either body is our target
    if (((layer1 == sm3d_Layers_ENEMY && layer2 == sm3d_Layers_PLAYER) || 
        (layer1 == sm3d_Layers_PLAYER &&
         layer2 == sm3d_Layers_ENEMY)) && globalPlayer->healthTimer <= 0.0f)
    {
        globalPlayer->healthTimer = 0.5f;
        globalPlayer->hurt = true;
        globalPlayer->hurtDamage = 3;
        JPH_Vec3 id1Pos;
        JPH_Vec3 id2Pos;

        JPH_BodyInterface_GetPosition(sm3d_state.bodyInterfaceNoLock, id1, &id1Pos);
        JPH_BodyInterface_GetPosition(sm3d_state.bodyInterfaceNoLock, id2, &id2Pos);

        JPH_Vec3_Subtract(&id1Pos, &id2Pos, &hurtDir);
        JPH_Vec3_Normalize(&hurtDir, &hurtDir);
    }

    return JPH_ValidateResult_AcceptAllContactsForThisBodyPair;
}

void Player_EnemyContactAdded(void* userData, const JPH_Body* body1,
                              const JPH_Body*            body2,
                              const JPH_ContactManifold* manifold,
                              JPH_ContactSettings*       settings)
{
}

void Player_EnemyContactPersisted(void*                      userData,
                                  const JPH_Body*            body1,
                                  const JPH_Body*            body2,
                                  const JPH_ContactManifold* manifold,
                                  JPH_ContactSettings*       settings)
{
}

void Player_EnemyContactRemoved(
    void* userData, const JPH_SubShapeIDPair* subShapeIDPair)
{
}

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

        smImGui_Checkbox("IsGrounded", &player->grounded);

        smImGui_DragFloat2("HeartSpriteScale",
                           player->heartSpriteScale, 0.1f);

        smImGui_InputText("Slide Sound", player->slideSoundPath, 128,
                          0);
        smImGui_InputText("Hurt Sound", player->hurtSoundPath, 128,
                          0);
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
    smJson_SaveVec2(j, "HeartSpriteScale", player->heartSpriteScale);
    smJson_SaveString(j, "SlideSound", player->slideSoundPath);
    smJson_SaveString(j, "HurtSound", player->hurtSoundPath);

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
    smJson_LoadVec2(j, "HeartSpriteScale", player->heartSpriteScale);
    smJson_LoadString(j, "SlideSound", player->slideSoundPath);
    smJson_LoadString(j, "HurtSound", player->hurtSoundPath);
}

void Player_StartSys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(Player))
    {
        Player* player = SM_ECS_GET(smState.scene, _entity, Player);

        globalPlayer = player;

        player->trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);
        player->rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);
        player->grounded = true;
        player->state = PlayerState_Flying;

        glm_vec3_copy((vec3) {0.0f, 1.0f, 0.0f},
                      player->groundNormal);

        playerData.health = 3;
        player->health = playerData.health;

        player->fullHeartSprite =
            smUtils_LoadTexture("res/textures/HeartSprite.png");
        player->emptyHeartSprite =
            smUtils_LoadTexture("res/textures/EmptyHeartSprite.png");

        player->slideSound =
            smAudio_LoadSound(player->slideSoundPath);
        player->hurtSound =
            smAudio_LoadSound(player->hurtSoundPath);

        player->audioSource = smAudioSource_Create(
            1.0f, 1.0f, GLM_VEC3_ZERO, GLM_VEC3_ZERO, false);
        player->slideAudioSource = smAudioSource_Create(
            1.0f, 1.0f, GLM_VEC3_ZERO, GLM_VEC3_ZERO, true);

        player->health = playerData.health;

        vec2 lastHeartPos = {40.0f, 40.0f};

        for (int i = 0; i < player->health; ++i)
        {
            smEntityID ent = smECS_AddEntity(smState.scene);
            smName* name = SM_ECS_ASSIGN(smState.scene, ent, smName);
            strcpy(name->name, "HeartImage");

            player->heartImages[i] =
                SM_ECS_ASSIGN(smState.scene, ent, smImage);
            strcpy(player->heartImages[i]->texturePath,
                   "res/textures/HeartSprite.png");
            player->heartImages[i]->texture = smUtils_LoadTexture(
                player->heartImages[i]->texturePath);

            glm_vec4_copy((vec4) {1.0f, 1.0f, 1.0f, 1.0f},
                          player->heartImages[i]->color);

            glm_vec2_copy(player->heartSpriteScale,
                          player->heartImages[i]->scale);

            glm_vec2_copy(lastHeartPos,
                          player->heartImages[i]->position);

            glm_vec2_add(lastHeartPos,
                         (vec2) {player->heartSpriteScale[0], 0.0f},
                         lastHeartPos);
            glm_vec2_add(lastHeartPos, (vec2) {25.0f, 0.0f},
                         lastHeartPos);
        }

        playerContactListener = JPH_ContactListener_Create((void*)9);

        playerContactListenerProcs.OnContactValidate =
            Player_EnemyContactValidate;
        playerContactListenerProcs.OnContactAdded =
            Player_EnemyContactAdded;
        playerContactListenerProcs.OnContactPersisted =
            Player_EnemyContactPersisted;
        playerContactListenerProcs.OnContactRemoved =
            Player_EnemyContactRemoved;

        JPH_ContactListener_SetProcs(&playerContactListenerProcs);

        JPH_PhysicsSystem_SetContactListener(sm3d_state.system,
                                             playerContactListener);
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
JPH_BodyID leafBodyID;

bool RaycastBodyFilterFunc(void* context, JPH_BodyID bodyID)
{
    JPH_ObjectLayer targetLayer = JPH_BodyInterface_GetObjectLayer(
        sm3d_state.bodyInterface, bodyID);

    JPH_BodyInterface_GetPosition(sm3d_state.bodyInterface, bodyID,
                                  &leafPos);
    leafBodyID = bodyID;

    // Only allow rays to hit the specific layer
    return sm3d_Layers_LEAF == targetLayer;
}
        
void Player_Hurt(Player* player, int damage)
{
    smAudioSource_PlaySound(&player->audioSource,
                            player->hurtSound);
    player->health--;
    
    JPH_Vec3 zer = {0.0f, 0.0f, 0.0f};
    JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &zer);
    JPH_Vec3_MultiplyScalar(&hurtDir, 3000.0f, &hurtDir);
    JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                                   player->rigid->bodyID, &hurtDir);
    
    if (player->health <= 0)
    {
        sm_playing = false;
        smEditor_LoadScene("sample_scene.json");
        sm_playing = true;
        smECS_StartStartSystems();
        return;
    }

    player->heartImages[player->health]->texture =
        player->emptyHeartSprite;
}

bool Player_IsGrounded(Player* player)
{
    float capsuleHalfHeight = player->rigid->capsuleHeight * 0.5f;

    JPH_Vec3 rayStart = {
        .x = player->trans->position[0],
        .y = player->trans->position[1] - 
            capsuleHalfHeight - 0.1f, // Just below the capsule
        .z = player->trans->position[2]
    };
    JPH_Vec3 rayEnd = {
        .x = player->trans->position[0],
        .y = rayStart.y - 0.05f, 
        .z = player->trans->position[2]
    };

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

    printf("Rayhit: %d\n", rayHit);

    return rayHit;
}

void Player_Jump(Player* player)
{
    if (!player->grounded)
        return;

    JPH_Vec3 jumpVelocity = {0.0f, player->jumpSpeed, 0.0f};

    JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                               player->rigid->bodyID, &jumpVelocity);
}

void Player_Walk(Player* player, vec3 direction, float acceleration)
{
    if (smInput_GetKeyDown(SM_KEY_SPACE))
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

        if (smInput_GetKeyDown(SM_KEY_LEFT_CONTROL))
        {
            smAudioSource_PlaySound(&player->slideAudioSource,
                                    player->slideSound);
        }
        if (smInput_GetKeyUp(SM_KEY_LEFT_CONTROL))
        {
            smAudioSource_StopSound(&player->slideAudioSource,
                                    player->slideSound);
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

        player->gravityTimer -= smState.deltaTime;
        player->healthTimer -= smState.deltaTime;

        if (player->gravityTimer > -0.1f &&
            player->gravityTimer < 0.1f)
        {
            JPH_BodyInterface_SetGravityFactor(
                sm3d_state.bodyInterface, player->rigid->bodyID,
                1.0f);
        }

        if (player->hurt)
        {
            Player_Hurt(player, player->hurtDamage);
            player->hurt = false;
        }

        static float lastX = 0.0f;
        static float lastY = 0.0f;
            
        double xpos, ypos;
        glfwGetCursorPos(smState.window->window, &xpos, &ypos);

        // Calculate offset
        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos;
        
        // Update last position
        lastX = (float)xpos;
        lastY = (float)ypos;
        
        // Apply sensitivity
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        // Update camera angles
        smState.camera.yaw += xoffset;
        smState.camera.pitch += yoffset;
        
        // Constrain pitch to prevent flipping
        if (smState.camera.pitch > 89.0f)
            smState.camera.pitch = 89.0f;
        if (smState.camera.pitch < -89.0f)
            smState.camera.pitch = -89.0f;
        
        // Update camera vectors
        smCamera_UpdateVectors(&smState.camera);
    }
    SM_ECS_ITER_END();
}
