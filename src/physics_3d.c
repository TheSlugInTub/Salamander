#include <salamander/physics_3d.h>
#include <salamander/state.h>
#include <salamander/imgui_layer.h>
#include <salamander/renderer.h>
#include <stdio.h>

JPH_ObjectLayer sm3d_Layers_NON_MOVING = 0;
JPH_ObjectLayer sm3d_Layers_MOVING = 1;
JPH_ObjectLayer sm3d_Layers_NUM_LAYERS = 2;

JPH_BroadPhaseLayer sm3d_BroadPhaseLayers_NON_MOVING = 0;
JPH_BroadPhaseLayer sm3d_BroadPhaseLayers_MOVING = 1;
uint32_t            sm3d_BroadPhaseLayers_NUM_LAYERS = 2;

smPhysics3DState sm3d_state = {};

void smPhysics3D_TraceImpl(const char* message)
{
    // Print to the TTY
    printf("%s", message);
}

void smPhysics3D_Init()
{
    if (!JPH_Init())
    {
        printf("Jolt physics (Physics3D) failed to initialize\n");
    }

    JPH_SetTraceHandler(smPhysics3D_TraceImpl);
    // JPH_SetAssertFailureHandler(JPH_AssertFailureFunc handler);

    sm3d_state.jobSystem = JPH_JobSystemThreadPool_Create(NULL);

    // We use only 2 layers: one for non-moving objects and one for
    // moving objects
    sm3d_state.objectLayerPairFilterTable =
        JPH_ObjectLayerPairFilterTable_Create(2);

    JPH_ObjectLayerPairFilterTable_EnableCollision(
        sm3d_state.objectLayerPairFilterTable, sm3d_Layers_NON_MOVING,
        sm3d_Layers_MOVING);

    JPH_ObjectLayerPairFilterTable_EnableCollision(
        sm3d_state.objectLayerPairFilterTable, sm3d_Layers_MOVING,
        sm3d_Layers_NON_MOVING);

    // We use a 1-to-1 mapping between object layers and broadphase
    // layers
    sm3d_state.broadPhaseLayerInterfaceTable =
        JPH_BroadPhaseLayerInterfaceTable_Create(2, 2);

    JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
        sm3d_state.broadPhaseLayerInterfaceTable,
        sm3d_Layers_NON_MOVING, sm3d_BroadPhaseLayers_NON_MOVING);

    JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
        sm3d_state.broadPhaseLayerInterfaceTable, sm3d_Layers_MOVING,
        sm3d_BroadPhaseLayers_MOVING);

    sm3d_state.objectVsBroadPhaseLayerFilter =
        JPH_ObjectVsBroadPhaseLayerFilterTable_Create(
            sm3d_state.broadPhaseLayerInterfaceTable, 2,
            sm3d_state.objectLayerPairFilterTable, 2);

    sm3d_state.settings.maxBodies = 65536;
    sm3d_state.settings.numBodyMutexes = 0;
    sm3d_state.settings.maxBodyPairs = 65536;
    sm3d_state.settings.maxContactConstraints = 65536;
    sm3d_state.settings.broadPhaseLayerInterface =
        sm3d_state.broadPhaseLayerInterfaceTable;
    sm3d_state.settings.objectLayerPairFilter =
        sm3d_state.objectLayerPairFilterTable;
    sm3d_state.settings.objectVsBroadPhaseLayerFilter =
        sm3d_state.objectVsBroadPhaseLayerFilter;
    sm3d_state.system =
        JPH_PhysicsSystem_Create(&sm3d_state.settings);
    sm3d_state.bodyInterface =
        JPH_PhysicsSystem_GetBodyInterface(sm3d_state.system);
}

void smPhysics3D_Destroy()
{
    JPH_JobSystem_Destroy(sm3d_state.jobSystem);
    JPH_PhysicsSystem_Destroy(sm3d_state.system);
    JPH_Shutdown();
}

void smPhysics3D_Step()
{
    const float cDeltaTime = 1.0f / 60.0f;
    const int   cCollisionSteps = 1;

    JPH_PhysicsSystem_Update(sm3d_state.system, cDeltaTime,
                             cCollisionSteps, sm3d_state.jobSystem);
}

void smPhysics3D_CreateBody(smRigidbody3D* rigid, smTransform* trans)
{
    switch (rigid->colliderType)
    {
        case sm3d_Box:
        {
            JPH_Vec3 halfExtents = {rigid->boxHalfwidths[0],
                                    rigid->boxHalfwidths[1],
                                    rigid->boxHalfwidths[2]};
            JPH_Vec3 position = {trans->position[0],
                                 trans->position[1],
                                 trans->position[2]};

            JPH_BoxShape* shape = JPH_BoxShape_Create(
                &halfExtents, JPH_DEFAULT_CONVEX_RADIUS);

            JPH_BodyCreationSettings* settings =
                JPH_BodyCreationSettings_Create3(
                    (const JPH_Shape*)shape, &position, NULL,
                    rigid->bodyType == 0 ? JPH_MotionType_Static
                                         : JPH_MotionType_Dynamic,
                    rigid->bodyType == 0 ? sm3d_Layers_NON_MOVING
                                         : sm3d_Layers_MOVING);

            JPH_MassProperties msp = {};
            JPH_MassProperties_ScaleToMass(&msp, rigid->mass);
            JPH_BodyCreationSettings_SetMassPropertiesOverride(
                settings, &msp);
            JPH_BodyCreationSettings_SetOverrideMassProperties(
                settings,
                JPH_OverrideMassProperties_CalculateInertia);

            rigid->bodyID = JPH_BodyInterface_CreateAndAddBody(
                sm3d_state.bodyInterface, settings,
                rigid->bodyType == 0 ? JPH_Activation_DontActivate
                                     : JPH_Activation_Activate);

            JPH_BodyCreationSettings_Destroy(settings);
            break;
        }
        case sm3d_Sphere:
        {
            JPH_Vec3 position = {trans->position[0],
                                 trans->position[1],
                                 trans->position[2]};

            JPH_SphereShape* shape =
                JPH_SphereShape_Create(rigid->sphereRadius);

            JPH_BodyCreationSettings* settings =
                JPH_BodyCreationSettings_Create3(
                    (const JPH_Shape*)shape, &position, NULL,
                    rigid->bodyType == 0 ? JPH_MotionType_Static
                                         : JPH_MotionType_Dynamic,
                    rigid->bodyType == 0 ? sm3d_Layers_NON_MOVING
                                         : sm3d_Layers_MOVING);

            JPH_MassProperties msp = {};
            JPH_MassProperties_ScaleToMass(&msp, rigid->mass);
            JPH_BodyCreationSettings_SetMassPropertiesOverride(
                settings, &msp);
            JPH_BodyCreationSettings_SetOverrideMassProperties(
                settings,
                JPH_OverrideMassProperties_CalculateInertia);

            rigid->bodyID = JPH_BodyInterface_CreateAndAddBody(
                sm3d_state.bodyInterface, settings,
                rigid->bodyType == 0 ? JPH_Activation_DontActivate
                                     : JPH_Activation_Activate);

            JPH_BodyCreationSettings_Destroy(settings);
            break;
        }
        case sm3d_Capsule:
        {
            // TODO
            break;
        }
        case sm3d_Mesh:
        {
            // TODO
            break;
        }
    }
}

void smJPHQuaternionToEuler(JPH_Quat* quaternion, vec3 euler)
{
    float x = quaternion->x;
    float y = quaternion->y;
    float z = quaternion->z;
    float w = quaternion->w;

    // Roll (x-axis rotation)
    float sinr_cosp = 2 * (w * x + y * z);
    float cosr_cosp = 1 - 2 * (x * x + y * y);
    euler[0] = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2 * (w * y - z * x);
    if (fabsf(sinp) >= 1)
        euler[1] = copysignf(M_PI / 2,
                             sinp); // Use 90 degrees if out of range
    else
        euler[1] = asinf(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = 2 * (w * z + x * y);
    float cosy_cosp = 1 - 2 * (y * y + z * z);
    euler[2] = atan2f(siny_cosp, cosy_cosp);
}

void smRigidbody3D_StartSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody3D))
    {
        smRigidbody3D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);

        smPhysics3D_CreateBody(rigid, trans);
    }
    SM_ECS_ITER_END();
}

void smRigidbody3D_Sys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody3D))
    {
        smRigidbody3D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);

        JPH_RVec3 pos;
        JPH_Quat  rot;
        JPH_BodyInterface_GetPositionAndRotation(
            sm3d_state.bodyInterface, rigid->bodyID, &pos, &rot);

        trans->position[0] = pos.x;
        trans->position[1] = pos.y;
        trans->position[2] = pos.z;

        smJPHQuaternionToEuler(&rot, trans->rotation);
    }
    SM_ECS_ITER_END();
}

void smRigidbody3D_DebugSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody3D))
    {
        smRigidbody3D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);

        switch (rigid->colliderType)
        {
            case sm3d_Box:
            {
                vec3 points[16] = {};
                vec3 pos;
                vec3 halfwidths;
                glm_vec3_copy(rigid->boxHalfwidths, halfwidths);
                glm_vec3_copy(trans->position, pos);

                glm_vec3_copy((vec3) {pos[0] - halfwidths[0],
                                      pos[1] - halfwidths[1],
                                      pos[2] - halfwidths[2]},
                              points[0]); // bottom-left-back
                glm_vec3_copy((vec3) {pos[0] + halfwidths[0],
                                      pos[1] - halfwidths[1],
                                      pos[2] - halfwidths[2]},
                              points[1]); // bottom-right-back
                glm_vec3_copy((vec3) {pos[0] + halfwidths[0],
                                      pos[1] - halfwidths[1],
                                      pos[2] + halfwidths[2]},
                              points[2]); // bottom-right-front
                glm_vec3_copy((vec3) {pos[0] - halfwidths[0],
                                      pos[1] - halfwidths[1],
                                      pos[2] + halfwidths[2]},
                              points[3]); // bottom-left-front

                // Top face (four corners)
                glm_vec3_copy((vec3) {pos[0] - halfwidths[0],
                                      pos[1] + halfwidths[1],
                                      pos[2] - halfwidths[2]},
                              points[4]); // top-left-back
                glm_vec3_copy((vec3) {pos[0] + halfwidths[0],
                                      pos[1] + halfwidths[1],
                                      pos[2] - halfwidths[2]},
                              points[5]); // top-right-back
                glm_vec3_copy((vec3) {pos[0] + halfwidths[0],
                                      pos[1] + halfwidths[1],
                                      pos[2] + halfwidths[2]},
                              points[6]); // top-right-front
                glm_vec3_copy((vec3) {pos[0] - halfwidths[0],
                                      pos[1] + halfwidths[1],
                                      pos[2] + halfwidths[2]},
                              points[7]); // top-left-front

                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);
                smRenderer_RenderLine3D(
                    points, 8, (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f,
                    3.0f, true, smState.persProj, view);
                break;
            }
            case sm3d_Sphere:
            {
                vec3  points[16] = {};
                vec3  pos;
                float radius = rigid->sphereRadius;
                glm_vec3_copy(trans->position, pos);

                glm_vec3_copy((vec3) {pos[0] - radius,
                                      pos[1] - radius,
                                      pos[2] - radius},
                              points[0]); // bottom-left-back
                glm_vec3_copy((vec3) {pos[0] + radius,
                                      pos[1] - radius,
                                      pos[2] - radius},
                              points[1]); // bottom-right-back
                glm_vec3_copy((vec3) {pos[0] + radius,
                                      pos[1] - radius,
                                      pos[2] + radius},
                              points[2]); // bottom-right-front
                glm_vec3_copy((vec3) {pos[0] - radius,
                                      pos[1] - radius,
                                      pos[2] + radius},
                              points[3]); // bottom-left-front Top
                                          // face (four corners)
                glm_vec3_copy((vec3) {pos[0] - radius,
                                      pos[1] + radius,
                                      pos[2] - radius},
                              points[4]); // top-left-back
                glm_vec3_copy((vec3) {pos[0] + radius,
                                      pos[1] + radius,
                                      pos[2] - radius},
                              points[5]); // top-right-back
                glm_vec3_copy((vec3) {pos[0] + radius,
                                      pos[1] + radius,
                                      pos[2] + radius},
                              points[6]); // top-right-front
                glm_vec3_copy((vec3) {pos[0] - radius,
                                      pos[1] + radius,
                                      pos[2] + radius},
                              points[7]); // top-left-front
                
                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);
                smRenderer_RenderLine3D(
                    points, 8, (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f,
                    3.0f, true, smState.persProj, view);
                break;
            }
            case sm3d_Capsule:
            {
                break;
            }
            case sm3d_Mesh:
            {
                // TODO
                break;
            }
        }
    }
    SM_ECS_ITER_END();
}

void smRigidbody3D_Draw(smRigidbody3D* rigid)
{
    if (smImGui_CollapsingHeader("Rigidbody3D"))
    {
        int colTypeValue = rigid->colliderType;
        if (smImGui_SliderInt("sm3d ColliderType", &colTypeValue, 0,
                              3))
        {
            rigid->colliderType = colTypeValue;
        }

        int bodyTypeValue = rigid->bodyType;
        if (smImGui_SliderInt("sm3d BodyType", &bodyTypeValue, 0, 2))
        {
            rigid->bodyType = bodyTypeValue;
        }

        smImGui_DragFloat("sm3d Mass", &rigid->mass, 0.1f);
        smImGui_DragFloat("sm3d Friction", &rigid->friction, 0.1f);
        smImGui_DragFloat("sm3d Linear Damping",
                          &rigid->linearDamping, 0.1f);
        smImGui_DragFloat("sm3d Angular Damping",
                          &rigid->angularDamping, 0.1f);
        smImGui_DragFloat("sm3d Restitution", &rigid->restitution,
                          0.1f);

        switch (rigid->colliderType)
        {
            case sm3d_Box:
            {
                smImGui_DragFloat3("sm3d Halfwidths",
                                   rigid->boxHalfwidths, 0.1f);
                break;
            }
            case sm3d_Sphere:
            {
                smImGui_DragFloat("sm3d Radius", &rigid->sphereRadius,
                                  0.1f);
                break;
            }
            case sm3d_Capsule:
            {
                smImGui_DragFloat("sm3d CapsuleRadius",
                                  &rigid->capsuleRadius, 0.1f);
                smImGui_DragFloat("sm3d CapsuleHeight",
                                  &rigid->capsuleHeight, 0.1f);
            }
            case sm3d_Mesh:
            {
                // TODO
            }
        }
    }
}

smJson smRigidbody3D_Save(smRigidbody3D* rigid)
{
    smJson j = smJson_Create();

    smJson_SaveFloat(j, "Mass", rigid->mass);
    smJson_SaveFloat(j, "LinearDamping", rigid->linearDamping);
    smJson_SaveFloat(j, "AngularDamping", rigid->angularDamping);
    smJson_SaveFloat(j, "Restitution", rigid->restitution);
    smJson_SaveInt(j, "BodyType", rigid->bodyType);
    smJson_SaveInt(j, "ColliderType", rigid->colliderType);

    switch (rigid->colliderType)
    {
        case sm3d_Box:
        {
            smJson_SaveVec3(j, "BoxHalfwidths", rigid->boxHalfwidths);
        }
        case sm3d_Sphere:
        {
            smJson_SaveFloat(j, "SphereRadius", rigid->sphereRadius);
        }
        case sm3d_Capsule:
        {
            smJson_SaveFloat(j, "CapsuleRadius",
                             rigid->capsuleRadius);
            smJson_SaveFloat(j, "CapsuleHeight",
                             rigid->capsuleHeight);
        }
        case sm3d_Mesh:
        {
            // TODO
        }
    }

    return j;
}

void smRigidbody3D_Load(smRigidbody3D* rigid, smJson j)
{
    smJson_LoadFloat(j, "Mass", &rigid->mass);
    smJson_LoadFloat(j, "LinearDamping", &rigid->linearDamping);
    smJson_LoadFloat(j, "AngularDamping", &rigid->angularDamping);
    smJson_LoadFloat(j, "Restitution", &rigid->restitution);
    smJson_LoadInt(j, "BodyType", &rigid->bodyType);
    smJson_LoadInt(j, "ColliderType", &rigid->colliderType);

    switch (rigid->colliderType)
    {
        case sm3d_Box:
        {
            smJson_LoadVec3(j, "BoxHalfwidths", rigid->boxHalfwidths);
            break;
        }
        case sm3d_Sphere:
        {
            smJson_LoadFloat(j, "SphereRadius", &rigid->sphereRadius);
            break;
        }
        case sm3d_Capsule:
        {
            smJson_LoadFloat(j, "CapsuleRadius",
                             &rigid->capsuleRadius);
            smJson_LoadFloat(j, "CapsuleHeight",
                             &rigid->capsuleHeight);
            break;
        }
        case sm3d_Mesh:
        {
            // TODO
            break;
        }
    }
}
