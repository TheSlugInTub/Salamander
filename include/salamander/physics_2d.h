#pragma once

#include <box2d/box2d.h>
#include <salamander/components.h>
#include <salamander/vector.h>
#include <salamander/json_api.h>

typedef enum
{
    sm2d_Static,
    sm2d_Dynamic,
    sm2d_Kinematic
} smRigidbody2DType;

typedef enum
{
    sm2d_Box,
    sm2d_Circle,
    sm2d_Polygon
} smCollider2DType;

typedef struct
{
    int          type;
    smTransform* transform;

    float mass;
    float friction;
    float linearDamping;
    float angularDamping;
    float restitution;
    bool  fixedRotation;
    bool  alwaysAwake;

    b2BodyDef bodyDef;
    b2BodyId  bodyID;
} smRigidbody2D;

typedef struct
{
    int colliderType;

    smRigidbody2D* body;

    unsigned int categoryBits;
    unsigned int maskBits;

    float     radius;     // Circle
    vec2      halfwidths; // Box
    smVector* points;     // Polygon

    b2ShapeDef shapeDef;
    b2ShapeId  shapeID;
    b2Polygon  polygon;
    b2Circle   circle;
} smCollider2D;

typedef enum
{
    sm2d_Revolute,
    sm2d_Distance
} smJoint2DType;

typedef struct
{
    smJoint2DType type;
    b2BodyId      bodyA;
    b2BodyId      bodyB;

    vec2 anchorA;
    vec2 anchorB;

    float lowerAngle;
    float upperAngle;
    float distance;

    b2RevoluteJointDef revoluteJointDef;
    b2DistanceJointDef distanceJointDef;
    b2JointId          jointID;
} smJoint2D;

typedef struct
{
    b2BodyId  collidingBody;
    b2ShapeId collidingShape;
} smShapeCastContext2D;

extern b2WorldDef sm_worldDef;
extern b2WorldId  sm_worldID;

void smPhysics2D_Init();
void smPhysics2D_Step();
void smPhysics2D_Destroy();

void smCollider2D_DebugSys();

void smPhysics2D_CreateRevoluteJoint(smJoint2D* joint, vec2 anchorA,
                                     vec2 anchorB, b2BodyId bodyA,
                                     b2BodyId bodyB, float lowerAngle,
                                     float upperAngle);

void smPhysics2D_smCreateDistanceJoint(smJoint2D* joint, vec2 anchorA,
                                       vec2 anchorB, b2BodyId bodyA,
                                       b2BodyId bodyB,
                                       float    distance);

void smRigidbody2D_StartSys();
void smCollider2D_StartSys();
void smRigidbody2D_FixCollidersStartSys();

void smRigidbody2D_Sys();
void smCollider2D_DebugSys();

void   smRigidbody2D_Draw(smRigidbody2D* rb);
smJson smRigidbody2D_Save(smRigidbody2D* rb);
void   smRigidbody2D_Load(smRigidbody2D* rb, smJson j);

void   smCollider2D_Draw(smCollider2D* col);
smJson smCollider2D_Save(smCollider2D* col);
void   smCollider2D_Load(smCollider2D* col, smJson j);
