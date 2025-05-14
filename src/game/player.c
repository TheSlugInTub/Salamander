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
        smImGui_DragFloat("DashSpeed", &player->dashSpeed, 0.1f);
        smImGui_DragFloat("WalkDashSpeed", &player->walkDashSpeed,
                          0.1f);
        smImGui_DragFloat("WalkDashAirTime", &player->walkDashAirTime,
                          0.1f);
        smImGui_DragFloat("LeafRegenSpeed", &player->leafRegenSpeed,
                          0.1f);
        smImGui_DragFloat2("LeafSpriteScale", player->leafSpriteScale,
                           0.1f);

        smImGui_Checkbox("IsGrounded", &player->grounded);

        smImGui_DragFloat2("HeartSpriteScale",
                           player->heartSpriteScale, 0.1f);

        smImGui_InputText("Leaf Throw Sound",
                          player->leafThrowSoundPath, 128, 0);
        smImGui_InputText("Leaf Dash Sound",
                          player->leafDashSoundPath, 128, 0);
        smImGui_InputText("Dash Sound", player->dashSoundPath, 128,
                          0);
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
    smJson_SaveFloat(j, "DashSpeed", player->dashSpeed);
    smJson_SaveFloat(j, "WalkDashSpeed", player->walkDashSpeed);
    smJson_SaveFloat(j, "WalkDashAirTime", player->walkDashAirTime);
    smJson_SaveFloat(j, "LeafRegenSpeed", player->leafRegenSpeed);
    smJson_SaveVec2(j, "LeafSpriteScale", player->leafSpriteScale);
    smJson_SaveVec2(j, "HeartSpriteScale", player->heartSpriteScale);
    smJson_SaveString(j, "LeafThrowSound",
                      player->leafThrowSoundPath);
    smJson_SaveString(j, "LeafDashSound", player->leafDashSoundPath);
    smJson_SaveString(j, "DashSound", player->dashSoundPath);
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
    smJson_LoadFloat(j, "DashSpeed", &player->dashSpeed);
    smJson_LoadFloat(j, "WalkDashSpeed", &player->walkDashSpeed);
    smJson_LoadFloat(j, "WalkDashAirTime", &player->walkDashAirTime);
    smJson_LoadFloat(j, "LeafRegenSpeed", &player->leafRegenSpeed);
    smJson_LoadVec2(j, "LeafSpriteScale", player->leafSpriteScale);
    smJson_LoadVec2(j, "HeartSpriteScale", player->heartSpriteScale);
    smJson_LoadString(j, "LeafThrowSound",
                      player->leafThrowSoundPath);
    smJson_LoadString(j, "LeafDashSound", player->leafDashSoundPath);
    smJson_LoadString(j, "DashSound", player->dashSoundPath);
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

        playerData.leafCount = 3;
        player->currentLeafCount = playerData.leafCount;
        playerData.health = 3;
        player->health = playerData.health;

        player->fullLeafSprite =
            smUtils_LoadTexture("res/textures/LeafSprite.png");
        player->emptyLeafSprite =
            smUtils_LoadTexture("res/textures/EmptyLeafSprite.png");

        player->fullHeartSprite =
            smUtils_LoadTexture("res/textures/HeartSprite.png");
        player->emptyHeartSprite =
            smUtils_LoadTexture("res/textures/EmptyHeartSprite.png");

        player->leafThrowSound =
            smAudio_LoadSound(player->leafThrowSoundPath);
        player->leafDashSound =
            smAudio_LoadSound(player->leafDashSoundPath);
        player->dashSound = smAudio_LoadSound(player->dashSoundPath);
        player->slideSound =
            smAudio_LoadSound(player->slideSoundPath);
        player->hurtSound =
            smAudio_LoadSound(player->hurtSoundPath);

        player->audioSource = smAudioSource_Create(
            1.0f, 1.0f, GLM_VEC3_ZERO, GLM_VEC3_ZERO, false);
        player->slideAudioSource = smAudioSource_Create(
            1.0f, 1.0f, GLM_VEC3_ZERO, GLM_VEC3_ZERO, true);

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

        player->health = playerData.health;

        vec2 lastHeartPos = {40.0f, 140.0f};

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

void Player_LeafDash(Player* player)
{
    JPH_Vec3 rayStart = {.x = smState.camera.position[0],
                         .y = smState.camera.position[1],
                         .z = smState.camera.position[2]};

    vec3 camFront;
    glm_vec3_copy(smState.camera.front, camFront);
    glm_vec3_mul(camFront, (vec3) {1000000.0f, 1000000.0f, 1000000.0f},
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
        smAudioSource_PlaySound(&player->audioSource,
                                player->leafDashSound);

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

        JPH_BodyInterface_RemoveAndDestroyBody(
            sm3d_state.bodyInterface, leafBodyID);
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

    JPH_Vec3 jumpVelocity = {0.0f, player->jumpSpeed, 0.0f};

    JPH_BodyInterface_AddForce(sm3d_state.bodyInterface,
                               player->rigid->bodyID, &jumpVelocity);
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

void Player_Dash(Player* player, vec3 direction)
{
    smAudioSource_PlaySound(&player->audioSource, player->dashSound);

    // Normalize the direction vector
    JPH_Vec3 normalizedDir = {direction[0], direction[1],
                              direction[2]};

    // Calculate the dash force
    JPH_Vec3 dashForce;
    JPH_Vec3_MultiplyScalar(&normalizedDir, player->walkDashSpeed,
                            &dashForce);

    // First, stop current velocity
    JPH_Vec3 zeroVelocity = {0.0f, 0.0f, 0.0f};
    JPH_BodyInterface_SetLinearVelocity(sm3d_state.bodyInterface,
                                        player->rigid->bodyID,
                                        &zeroVelocity);

    // Apply the dash force
    JPH_BodyInterface_AddImpulse(sm3d_state.bodyInterface,
                                 player->rigid->bodyID, &dashForce);

    // Disable gravity for the dash duration
    JPH_BodyInterface_SetGravityFactor(sm3d_state.bodyInterface,
                                       player->rigid->bodyID, 0.0f);

    player->gravityTimer = player->walkDashAirTime;
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

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_LEFT))
        {
            Player_LeafDash(player);
        }

        if (smInput_GetKeyDown(SM_KEY_LEFT_SHIFT) &&
            player->currentLeafCount > 0 &&
            player->gravityTimer <= 0.0f)
        {
            Player_Dash(player, moveDirection);
            player->currentLeafCount--;
            player->leafRegenTimer = player->leafRegenSpeed;
            player->leafImages[player->currentLeafCount]->texture =
                player->emptyLeafSprite;
        }

        player->leafRegenTimer -= smState.deltaTime;
        player->gravityTimer -= smState.deltaTime;
        player->healthTimer -= smState.deltaTime;

        if (player->gravityTimer > -0.1f &&
            player->gravityTimer < 0.1f)
        {
            JPH_BodyInterface_SetGravityFactor(
                sm3d_state.bodyInterface, player->rigid->bodyID,
                1.0f);
        }

        if (player->leafRegenTimer <= 0.0f &&
            player->currentLeafCount < playerData.leafCount &&
            JPH_Vec3_Length(&curVelocity) > 2.0f)
        {
            player->leafImages[player->currentLeafCount]->texture =
                player->fullLeafSprite;

            player->leafRegenTimer = player->leafRegenSpeed;
            player->currentLeafCount++;
        }

        if (player->hurt)
        {
            Player_Hurt(player, player->hurtDamage);
            player->hurt = false;
        }

        if (smInput_GetMouseButtonDown(SM_MOUSE_BUTTON_RIGHT) &&
            player->currentLeafCount > 0)
        {
            smAudioSource_PlaySound(&player->audioSource,
                                    player->leafThrowSound);

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

            smPhysics3D_CreateBody(leafBody, leafTrans, NULL);

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
