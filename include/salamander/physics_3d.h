#pragma once

#include <jolt/joltc.h>
#include <salamander/glm.h>
#include <salamander/components.h>
#include <salamander/json_api.h>

typedef struct
{
    JPH_JobSystem*                     jobSystem;
    JPH_ObjectLayerPairFilter*         objectLayerPairFilterTable;
    JPH_BroadPhaseLayerInterface*      broadPhaseLayerInterfaceTable;
    JPH_ObjectVsBroadPhaseLayerFilter* objectVsBroadPhaseLayerFilter;

    JPH_PhysicsSystemSettings settings;
    JPH_PhysicsSystem*        system;
    JPH_BodyInterface*        bodyInterface;
} smPhysics3DState;

typedef enum
{
    sm3d_Static,
    sm3d_Dynamic,
    sm3d_Kinematic
} smRigidbody3DType;

typedef enum
{
    sm3d_Box,
    sm3d_Capsule,
    sm3d_Sphere,
    sm3d_Mesh
} smCollider3DType;

typedef struct
{
    int bodyType;
    int colliderType;

    float mass;
    float friction;
    float linearDamping;
    float angularDamping;
    float restitution;

    vec3  boxHalfwidths;                // Box
    float sphereRadius;                 // Sphere
    float capsuleRadius, capsuleHeight; // Capsule

    JPH_BodyID bodyID;
} smRigidbody3D;

extern smPhysics3DState sm3d_state;

extern JPH_ObjectLayer sm3d_Layers_NON_MOVING;
extern JPH_ObjectLayer sm3d_Layers_MOVING;
extern JPH_ObjectLayer sm3d_Layers_NUM_LAYERS;

extern JPH_BroadPhaseLayer sm3d_BroadPhaseLayers_NON_MOVING;
extern JPH_BroadPhaseLayer sm3d_BroadPhaseLayers_MOVING;
extern uint32_t            sm3d_BroadPhaseLayers_NUM_LAYERS;

void smPhysics3D_TraceImpl(const char* message);
void smPhysics3D_Init();
void smPhysics3D_Step();
void smPhysics3D_Destroy();
void smPhysics3D_ClearWorld();

void smPhysics3D_CreateBody(smRigidbody3D* rigid, smTransform* trans);

void smRigidbody3D_Sys();
void smRigidbody3D_StartSys();
void smRigidbody3D_DebugSys();

void   smRigidbody3D_Draw(smRigidbody3D* rigid);
smJson smRigidbody3D_Save(smRigidbody3D* rigid);
void   smRigidbody3D_Load(smRigidbody3D* rigid, smJson j);
