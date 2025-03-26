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

    JPH_ObjectLayerPairFilterTable_EnableCollision(
        sm3d_state.objectLayerPairFilterTable, sm3d_Layers_MOVING,
        sm3d_Layers_MOVING);

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

void smPhysics3D_ClearWorld()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody3D))
    {
        smRigidbody3D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody3D);

        JPH_BodyInterface_RemoveAndDestroyBody(
            sm3d_state.bodyInterface, rigid->bodyID);
    }
    SM_ECS_ITER_END();
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

            vec4 glmQuat;

            glm_euler_xyz_quat(trans->rotation, glmQuat);

            JPH_Quat quat = (JPH_Quat) {glmQuat[0], glmQuat[1],
                                        glmQuat[2], glmQuat[3]};

            JPH_BodyCreationSettings* settings =
                JPH_BodyCreationSettings_Create3(
                    (const JPH_Shape*)shape, &position, &quat,
                    rigid->bodyType == 0 ? JPH_MotionType_Static
                                         : JPH_MotionType_Dynamic,
                    rigid->bodyType == 0 ? sm3d_Layers_NON_MOVING
                                         : sm3d_Layers_MOVING);

            JPH_MassProperties msp = {};

            // Lock rotation
            if (rigid->fixedRotation)
            {
                JPH_BodyCreationSettings_SetAllowedDOFs(
                    settings, JPH_AllowedDOFs_TranslationX |
                                  JPH_AllowedDOFs_TranslationY |
                                  JPH_AllowedDOFs_TranslationZ);
            }

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

            // Lock rotation
            if (rigid->fixedRotation)
            {
                JPH_BodyCreationSettings_SetAllowedDOFs(
                    settings, JPH_AllowedDOFs_TranslationX |
                                  JPH_AllowedDOFs_TranslationY |
                                  JPH_AllowedDOFs_TranslationZ);
            }

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
            JPH_Vec3 position = {trans->position[0],
                                 trans->position[1],
                                 trans->position[2]};

            // Create capsule shape
            // Parameters: radius, half height (from center to end of
            // capsule)
            JPH_CapsuleShape* shape = JPH_CapsuleShape_Create(
                rigid->capsuleRadius,       // radius
                rigid->capsuleHeight * 0.5f // half height
            );

            // Quaternion for rotation
            vec4 glmQuat;
            glm_euler_xyz_quat(trans->rotation, glmQuat);

            JPH_Quat quat = (JPH_Quat) {glmQuat[0], glmQuat[1],
                                        glmQuat[2], glmQuat[3]};

            // Create body creation settings
            JPH_BodyCreationSettings* settings =
                JPH_BodyCreationSettings_Create3(
                    (const JPH_Shape*)shape, &position, &quat,
                    rigid->bodyType == 0 ? JPH_MotionType_Static
                                         : JPH_MotionType_Dynamic,
                    rigid->bodyType == 0 ? sm3d_Layers_NON_MOVING
                                         : sm3d_Layers_MOVING);

            // Set mass properties
            JPH_MassProperties msp = {};
            JPH_MassProperties_ScaleToMass(&msp, rigid->mass);

            // Lock rotation
            if (rigid->fixedRotation)
            {
                JPH_BodyCreationSettings_SetAllowedDOFs(
                    settings, JPH_AllowedDOFs_TranslationX |
                                  JPH_AllowedDOFs_TranslationY |
                                  JPH_AllowedDOFs_TranslationZ);
            }

            JPH_BodyCreationSettings_SetMassPropertiesOverride(
                settings, &msp);
            JPH_BodyCreationSettings_SetOverrideMassProperties(
                settings,
                JPH_OverrideMassProperties_CalculateInertia);

            // Create and add body
            rigid->bodyID = JPH_BodyInterface_CreateAndAddBody(
                sm3d_state.bodyInterface, settings,
                rigid->bodyType == 0 ? JPH_Activation_DontActivate
                                     : JPH_Activation_Activate);

            // Clean up
            JPH_BodyCreationSettings_Destroy(settings);
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

        // Create rotation matrix from euler angles
        mat4 rotMat;
        glm_mat4_identity(rotMat);

        // Apply rotations in ZYX order (common for euler angles)
        glm_rotate_x(rotMat, trans->rotation[0], rotMat);
        glm_rotate_y(rotMat, trans->rotation[1], rotMat);
        glm_rotate_z(rotMat, trans->rotation[2], rotMat);

        switch (rigid->colliderType)
        {
            case sm3d_Box:
            {
                vec3 points[8] = {};
                vec3 pos;
                vec3 halfwidths;
                glm_vec3_copy(rigid->boxHalfwidths, halfwidths);
                glm_vec3_copy(trans->position, pos);

                // Define box corners in local space (centered at
                // origin)
                vec3 localPoints[8] = {
                    {-halfwidths[0], -halfwidths[1],
                     -halfwidths[2]}, // bottom-left-back
                    {+halfwidths[0], -halfwidths[1],
                     -halfwidths[2]}, // bottom-right-back
                    {+halfwidths[0], -halfwidths[1],
                     +halfwidths[2]}, // bottom-right-front
                    {-halfwidths[0], -halfwidths[1],
                     +halfwidths[2]}, // bottom-left-front
                    {-halfwidths[0], +halfwidths[1],
                     -halfwidths[2]}, // top-left-back
                    {+halfwidths[0], +halfwidths[1],
                     -halfwidths[2]}, // top-right-back
                    {+halfwidths[0], +halfwidths[1],
                     +halfwidths[2]}, // top-right-front
                    {-halfwidths[0], +halfwidths[1],
                     +halfwidths[2]} // top-left-front
                };

                // Apply rotation and translation to each point
                for (int i = 0; i < 8; i++)
                {
                    vec3 rotated;
                    vec4 tmp = {localPoints[i][0], localPoints[i][1],
                                localPoints[i][2], 1.0f};
                    vec4 result;

                    // Apply rotation
                    glm_mat4_mulv(rotMat, tmp, result);

                    // Convert back to vec3 and add position
                    rotated[0] = result[0] + pos[0];
                    rotated[1] = result[1] + pos[1];
                    rotated[2] = result[2] + pos[2];

                    glm_vec3_copy(rotated, points[i]);
                }

                unsigned int indices[] = {
                    0, 1, // Front face, bottom edge
                    1, 2, // Front face, right edge
                    2, 3, // Front face, top edge
                    3, 0, // Front face, left edge
                    4, 5, // Back face, bottom edge
                    5, 6, // Back face, right edge
                    6, 7, // Back face, top edge
                    7, 4, // Back face, left edge
                    0, 4, // Bottom face, left edge
                    1, 5, // Bottom face, right edge
                    2, 6, // Top face, right edge
                    3, 7  // Top face, left edge
                };

                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);
                smRenderer_RenderIndexedLine3D(
                    points, 8, indices, 24,
                    (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f, 3.0f, true,
                    smState.persProj, view);
                break;
            }
            case sm3d_Sphere:
            {
                // For a sphere, rotation doesn't change the visual
                // appearance But we'll still draw a more structured
                // representation to help visualize rotation
                vec3  pos;
                float radius = rigid->sphereRadius;
                glm_vec3_copy(trans->position, pos);

                // Create a cubic frame around the sphere
                vec3 localPoints[8] = {
                    {-radius, -radius, -radius}, // bottom-left-back
                    {+radius, -radius, -radius}, // bottom-right-back
                    {+radius, -radius, +radius}, // bottom-right-front
                    {-radius, -radius, +radius}, // bottom-left-front
                    {-radius, +radius, -radius}, // top-left-back
                    {+radius, +radius, -radius}, // top-right-back
                    {+radius, +radius, +radius}, // top-right-front
                    {-radius, +radius, +radius}  // top-left-front
                };

                // Add axis indicators to show rotation
                vec3 axisPoints[6] = {
                    {0, 0, 0},             // Origin
                    {radius * 1.5f, 0, 0}, // X axis
                    {0, 0, 0},             // Origin
                    {0, radius * 1.5f, 0}, // Y axis
                    {0, 0, 0},             // Origin
                    {0, 0, radius * 1.5f}  // Z axis
                };

                // Apply rotation to all points
                vec3 points[8];
                vec3 rotatedAxisPoints[6];

                // Rotate and translate cube frame
                for (int i = 0; i < 8; i++)
                {
                    vec3 rotated;
                    vec4 tmp = {localPoints[i][0], localPoints[i][1],
                                localPoints[i][2], 1.0f};
                    vec4 result;

                    // Apply rotation
                    glm_mat4_mulv(rotMat, tmp, result);

                    // Convert back to vec3 and add position
                    rotated[0] = result[0] + pos[0];
                    rotated[1] = result[1] + pos[1];
                    rotated[2] = result[2] + pos[2];

                    glm_vec3_copy(rotated, points[i]);
                }

                // Rotate and translate axis indicators
                for (int i = 0; i < 6; i++)
                {
                    vec3 rotated;
                    vec4 tmp = {axisPoints[i][0], axisPoints[i][1],
                                axisPoints[i][2], 1.0f};
                    vec4 result;

                    // Apply rotation
                    glm_mat4_mulv(rotMat, tmp, result);

                    // Convert back to vec3 and add position
                    rotated[0] = result[0] + pos[0];
                    rotated[1] = result[1] + pos[1];
                    rotated[2] = result[2] + pos[2];

                    glm_vec3_copy(rotated, rotatedAxisPoints[i]);
                }

                unsigned int indices[] = {
                    0, 1, // Front face, bottom edge
                    1, 2, // Front face, right edge
                    2, 3, // Front face, top edge
                    3, 0, // Front face, left edge
                    4, 5, // Back face, bottom edge
                    5, 6, // Back face, right edge
                    6, 7, // Back face, top edge
                    7, 4, // Back face, left edge
                    0, 4, // Bottom face, left edge
                    1, 5, // Bottom face, right edge
                    2, 6, // Top face, right edge
                    3, 7  // Top face, left edge
                };

                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);

                // Draw cube frame
                smRenderer_RenderIndexedLine3D(
                    points, 8, indices, 24,
                    (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f, 3.0f, true,
                    smState.persProj, view);
                break;
            }
            case sm3d_Capsule:
            {
                vec3  pos;
                float radius = rigid->capsuleRadius;
                float height = rigid->capsuleHeight;
                glm_vec3_copy(trans->position, pos);

                // Calculate half-height (distance from center to end
                // of cylinder part)
                float halfHeight = height * 0.5f;

                // Create local points for capsule (8 points for each
                // end, plus 4 midline points)
                vec3 localPoints[12] = {
                    // Bottom hemisphere points
                    {-radius, -halfHeight,
                     -radius}, // 0: bottom-left-back
                    {+radius, -halfHeight,
                     -radius}, // 1: bottom-right-back
                    {+radius, -halfHeight,
                     +radius}, // 2: bottom-right-front
                    {-radius, -halfHeight,
                     +radius}, // 3: bottom-left-front

                    // Top hemisphere points
                    {-radius, +halfHeight,
                     -radius}, // 4: top-left-back
                    {+radius, +halfHeight,
                     -radius}, // 5: top-right-back
                    {+radius, +halfHeight,
                     +radius}, // 6: top-right-front
                    {-radius, +halfHeight,
                     +radius}, // 7: top-left-front

                    // Midline points for additional structure
                    {0, -halfHeight, 0}, // 8: bottom center
                    {0, +halfHeight, 0}, // 9: top center
                    {0, 0, -radius},     // 10: side point back
                    {0, 0, +radius}      // 11: side point front
                };

                // Add axis indicators to show rotation
                vec3 axisPoints[6] = {
                    {0, 0, 0},             // Origin
                    {radius * 1.5f, 0, 0}, // X axis
                    {0, 0, 0},             // Origin
                    {0, height, 0},        // Y axis (full height)
                    {0, 0, 0},             // Origin
                    {0, 0, radius * 1.5f}  // Z axis
                };

                // Rotate and translate points
                vec3 points[12];
                vec3 rotatedAxisPoints[6];

                // Rotate and translate capsule frame points
                for (int i = 0; i < 12; i++)
                {
                    vec4 tmp = {localPoints[i][0], localPoints[i][1],
                                localPoints[i][2], 1.0f};
                    vec4 result;

                    // Apply rotation
                    glm_mat4_mulv(rotMat, tmp, result);

                    // Convert back to vec3 and add position
                    vec3 rotated = {result[0] + pos[0],
                                    result[1] + pos[1],
                                    result[2] + pos[2]};

                    glm_vec3_copy(rotated, points[i]);
                }

                // Rotate and translate axis indicators (same as
                // sphere implementation)
                for (int i = 0; i < 6; i++)
                {
                    vec4 tmp = {axisPoints[i][0], axisPoints[i][1],
                                axisPoints[i][2], 1.0f};
                    vec4 result;

                    // Apply rotation
                    glm_mat4_mulv(rotMat, tmp, result);

                    // Convert back to vec3 and add position
                    vec3 rotated = {result[0] + pos[0],
                                    result[1] + pos[1],
                                    result[2] + pos[2]};

                    glm_vec3_copy(rotated, rotatedAxisPoints[i]);
                }

                // Indices to create wireframe representation
                unsigned int indices[] = {
                    // Bottom face wireframe
                    0, 1, 1, 2, 2, 3, 3, 0,

                    // Top face wireframe
                    4, 5, 5, 6, 6, 7, 7, 4,

                    // Vertical connecting lines
                    0, 4, 1, 5, 2, 6, 3, 7,

                    // Additional structural lines
                    8, 9,   // Vertical center line
                    10, 11, // Side cross line

                    // Connecting lines to midpoints
                    8, 0, 8, 1, 8, 2, 8, 3, 9, 4, 9, 5, 9, 6, 9, 7};

                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);

                // Draw capsule frame
                smRenderer_RenderIndexedLine3D(
                    points, 12, indices,
                    40, // Note increased index count
                    (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f, 3.0f, true,
                    smState.persProj, view);
                break;
            }
            case sm3d_Mesh:
            {
                // TODO: Implement mesh visualization with rotation
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
        smImGui_Checkbox("sm3d FixedRotation", &rigid->fixedRotation);

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
    smJson_SaveBool(j, "FixedRotation", rigid->fixedRotation);

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
    smJson_LoadBool(j, "FixedRotation", &rigid->fixedRotation);

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
