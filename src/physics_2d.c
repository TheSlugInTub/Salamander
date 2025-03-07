#include <salamander/physics_2d.h>
#include <salamander/renderer.h>
#include <salamander/input.h>
#include <salamander/registry.h>
#include <stdlib.h>
#include <stdio.h>

b2WorldDef sm_worldDef;
b2WorldId  sm_worldID;

void smPhysics2D_Init()
{
    sm_worldDef = b2DefaultWorldDef();
    sm_worldDef.gravity = (b2Vec2) {0.0f, -7.5f};

    sm_worldID = b2CreateWorld(&sm_worldDef);

    b2World_EnableSleeping(sm_worldID, false);

    b2AABB bounds = {{-FLT_MAX, -FLT_MAX}, {FLT_MAX, FLT_MAX}};
}

void smRigidbody2DFixCollidersStartSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smCollider2D))
    {
        smRigidbody2D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody2D);
        smCollider2D* col =
            SM_ECS_GET(smState.scene, _entity, smCollider2D);
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);

        col->body = rigid;
        col->body->transform = trans;
        rigid->transform = trans;
    }
    SM_ECS_ITER_END();
}

void smRigidbody2DStartSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody2D))
    {
        smRigidbody2D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody2D);

        if (b2Body_IsValid(rigid->bodyID))
        {
            continue;
        }

        rigid->bodyDef = b2DefaultBodyDef();

        rigid->bodyDef.position =
            (b2Vec2) {rigid->transform->position[0],
                      rigid->transform->position[1]};

        b2Rot rot;
        rot.c = cos(rigid->transform->rotation[2]);
        rot.s = sin(rigid->transform->rotation[2]);
        rigid->bodyDef.rotation = rot;

        if (rigid->type == sm2d_Dynamic)
        {
            rigid->bodyDef.type = b2_dynamicBody;
            rigid->bodyDef.angularDamping = rigid->angularDamping;
            rigid->bodyDef.linearDamping = rigid->linearDamping;
            rigid->bodyDef.fixedRotation = rigid->fixedRotation;
            rigid->bodyDef.enableSleep = !rigid->alwaysAwake;
            rigid->bodyDef.userData = (void*)rigid;
        }

        rigid->bodyID = b2CreateBody(sm_worldID, &rigid->bodyDef);

        if (!b2Body_IsValid(rigid->bodyID))
        {
            printf("Body of %s is not valid\n",
                   SM_ECS_GET(smState.scene, _entity, smName)->name);
        }
    }
    SM_ECS_ITER_END();
}

void smRigidbody2DSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody2D))
    {
        smRigidbody2D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody2D);

        if (rigid->type == sm2d_Static)
            continue;

        b2Vec2 pos = b2Body_GetPosition(rigid->bodyID);
        b2Rot  rot = b2Body_GetRotation(rigid->bodyID);

        rigid->transform->position[0] = pos.x;
        rigid->transform->position[1] = pos.y;
        rigid->transform->rotation[2] = b2Rot_GetAngle(rot);
    }
    SM_ECS_ITER_END();
}

void smCollider2DStartSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smRigidbody2D))
    {
        smCollider2D* col =
            SM_ECS_GET(smState.scene, _entity, smCollider2D);
        smRigidbody2D* rigid = col->body;

        switch (col->colliderType)
        {
            case sm2d_Box:
            {
                col->polygon =
                    b2MakeBox(col->halfwidths[0], col->halfwidths[1]);
                col->shapeDef = b2DefaultShapeDef();

                if (col->body->type == sm2d_Dynamic)
                {
                    col->shapeDef.friction = rigid->friction;
                    col->shapeDef.density = rigid->mass;
                    col->shapeDef.restitution = rigid->restitution;
                }

                col->shapeDef.filter = b2DefaultFilter();
                col->shapeDef.userData = &col;

                col->shapeID = b2CreatePolygonShape(
                    col->body->bodyID, &col->shapeDef, &col->polygon);

                if (!b2Shape_IsValid(col->shapeID))
                    printf("Box shape of %s is not valid\n",
                           SM_ECS_GET(smState.scene, _entity, smName)
                               ->name);

                break;
            }
            case sm2d_Circle:
            {
                col->shapeDef = b2DefaultShapeDef();

                if (col->body->type == sm2d_Dynamic)
                {
                    col->shapeDef.friction = rigid->friction;
                    col->shapeDef.density = rigid->mass;
                    col->shapeDef.restitution = rigid->restitution;
                }

                col->shapeDef.filter = b2DefaultFilter();
                col->shapeDef.userData = &col;

                col->circle.radius = col->radius;
                col->circle.center =
                    (b2Vec2) {col->body->transform->position[0],
                              col->body->transform->position[1]};

                col->shapeID = b2CreateCircleShape(
                    col->body->bodyID, &col->shapeDef, &col->circle);

                if (!b2Shape_IsValid(col->shapeID))
                {
                    printf("Circle shape of %s is not valid\n",
                           SM_ECS_GET(smState.scene, _entity, smName)
                               ->name);
                }
                break;
            }
            case sm2d_Polygon:
            {
                b2Vec2 points[100] = {};
                vec2   pos = {col->body->transform->position[0],
                              col->body->transform->position[1]};

                for (int i = 0; i < col->points->size; ++i)
                {
                    float* point =
                        (float*)smVector_Get(col->points, i);
                    points[i] = (b2Vec2) {point[0], point[1]};
                }

                b2Hull hull =
                    b2ComputeHull(points, col->points->size);
                float radius = 0.0f;
                col->polygon = b2MakePolygon(&hull, radius);

                if (hull.count < 3)
                {
                    printf("Degenerate polygon hull was created\n");
                }

                col->shapeDef = b2DefaultShapeDef();

                if (col->body->type == sm2d_Dynamic)
                {
                    col->shapeDef.friction = rigid->friction;
                    col->shapeDef.density = rigid->mass;
                    col->shapeDef.restitution = rigid->restitution;
                }

                col->shapeDef.filter = b2DefaultFilter();
                col->shapeDef.userData = (void*)col;

                col->shapeID = b2CreatePolygonShape(
                    col->body->bodyID, &col->shapeDef, &col->polygon);

                if (!b2Shape_IsValid(col->shapeID))
                {
                    printf("Polygon shape of %s is not valid\n",
                           SM_ECS_GET(smState.scene, _entity, smName)
                               ->name);
                }

                if (col->points == NULL)
                {
                    col->points = smVector_Create(sizeof(vec2), 10);
                }

                break;
            }
        }
    }
    SM_ECS_ITER_END();
}

float sm_timeStep = 1.0f / 60.0f;
int   sm_subStepCount = 4;

void smPhysics2D_Step()
{
    b2World_Step(sm_worldID, sm_timeStep, sm_subStepCount);
}

void smPhysics2D_Destroy()
{
    b2DestroyWorld(sm_worldID);
}

void smPhysics2D_CreateRevoluteJoint(smJoint2D* joint, vec2 anchorA,
                                     vec2 anchorB, b2BodyId bodyA,
                                     b2BodyId bodyB, float lowerAngle,
                                     float upperAngle)
{
    joint->type = sm2d_Revolute;
    joint->bodyA = bodyA;
    joint->bodyB = bodyB;
    glm_vec2_copy(anchorA, joint->anchorA);
    glm_vec2_copy(anchorB, joint->anchorB);
    joint->lowerAngle = lowerAngle;
    joint->upperAngle = upperAngle;

    joint->revoluteJointDef = b2DefaultRevoluteJointDef();
    joint->revoluteJointDef.bodyIdA = bodyA;
    joint->revoluteJointDef.bodyIdB = bodyB;

    if (!b2Body_IsValid(bodyA))
    {
        printf("BodA ain't valid yo!\n");
    }
    if (!b2Body_IsValid(bodyB))
    {
        printf("BodB ain't valid yo!\n");
    }

    joint->revoluteJointDef.localAnchorA =
        (b2Vec2) {anchorA[0], anchorA[1]};
    joint->revoluteJointDef.localAnchorB =
        (b2Vec2) {anchorB[0], anchorB[1]};

    joint->revoluteJointDef.enableLimit = true;
    joint->revoluteJointDef.lowerAngle = lowerAngle;
    joint->revoluteJointDef.upperAngle = upperAngle;

    joint->jointID =
        b2CreateRevoluteJoint(sm_worldID, &joint->revoluteJointDef);
}

void smPhysics2D_smCreateDistanceJoint(smJoint2D* joint, vec2 anchorA,
                                       vec2 anchorB, b2BodyId bodyA,
                                       b2BodyId bodyB, float distance)
{
    joint->type = sm2d_Distance;
    joint->bodyA = bodyA;
    joint->bodyB = bodyB;
    glm_vec2_copy(anchorA, joint->anchorA);
    glm_vec2_copy(anchorB, joint->anchorB);
    joint->distance = distance;

    joint->distanceJointDef = b2DefaultDistanceJointDef();
    joint->distanceJointDef.bodyIdA = bodyA;
    joint->distanceJointDef.bodyIdB = bodyB;

    joint->distanceJointDef.localAnchorA =
        (b2Vec2) {anchorA[0], anchorA[1]};
    joint->distanceJointDef.localAnchorB =
        (b2Vec2) {anchorB[0], anchorB[1]};

    b2Vec2 anchorAb2 = b2Body_GetWorldPoint(
        bodyA, joint->distanceJointDef.localAnchorA);
    b2Vec2 anchorBb2 = b2Body_GetWorldPoint(
        bodyB, joint->distanceJointDef.localAnchorB);

    joint->distanceJointDef.length = b2Distance(anchorAb2, anchorBb2);
    joint->distanceJointDef.collideConnected = true;

    joint->jointID =
        b2CreateDistanceJoint(sm_worldID, &joint->distanceJointDef);
}

void smCollider2DDebugSys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smCollider2D))
    {
        smRigidbody2D* rigid =
            SM_ECS_GET(smState.scene, _entity, smRigidbody2D);
        smCollider2D* col =
            SM_ECS_GET(smState.scene, _entity, smCollider2D);
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);

        if (rigid == NULL)
        {
            continue;
        }

        if (col->body == NULL)
        {
            col->body = rigid;
        }

        if (col->body->transform == NULL)
        {
            col->body->transform = trans;
        }

        if (col->colliderType == sm2d_Circle)
        {
            vec2 topLeft, topRight, bottomLeft, bottomRight;
            vec2 points[4];

            // Calculate corner positions
            glm_vec2_sub(col->body->transform->position,
                         (vec2) {col->radius, col->radius}, topLeft);
            glm_vec2_add(col->body->transform->position,
                         (vec2) {col->radius, -col->radius},
                         topRight);
            glm_vec2_add(col->body->transform->position,
                         (vec2) {-col->radius, col->radius},
                         bottomRight);
            glm_vec2_add(col->body->transform->position,
                         (vec2) {col->radius, col->radius},
                         bottomLeft);

            // Assign to points array
            glm_vec2_copy(topLeft, points[0]);
            glm_vec2_copy(bottomRight, points[1]);
            glm_vec2_copy(bottomLeft, points[2]);
            glm_vec2_copy(topRight, points[3]);

            mat4 view;
            smCamera_GetViewMatrix(&smState.camera, view);
            smRenderer_RenderLine2D(
                points, 4, (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f,
                3.0f, true, smState.persProj, view);
        }
        else if (col->colliderType == sm2d_Box)
        {
            // Get the rotation angle from transform
            float rotation =
                col->body->transform->rotation[2]; // in radians
            float cosAngle = cosf(rotation);
            float sinAngle = sinf(rotation);

            // Define the half-width and half-height
            float hw = col->halfwidths[0];
            float hh = col->halfwidths[1];

            // Create relative corner positions (before rotation and
            // translation)
            vec2 relTopLeft = {-hw, hh};
            vec2 relTopRight = {hw, hh};
            vec2 relBottomRight = {hw, -hh};
            vec2 relBottomLeft = {-hw, -hh};

            // Get center position
            vec2 center = {col->body->transform->position[0],
                           col->body->transform->position[1]};

            // Create an array of four points
            vec2 points[4];

            // Calculate the rotated and translated positions directly
            // into the points array bottomLeft (points[0])
            vec2 rotated = {relBottomLeft[0] * cosAngle -
                                relBottomLeft[1] * sinAngle,
                            relBottomLeft[0] * sinAngle +
                                relBottomLeft[1] * cosAngle};
            points[0][0] = rotated[0] + center[0];
            points[0][1] = rotated[1] + center[1];

            // topLeft (points[1])
            rotated[0] =
                relTopLeft[0] * cosAngle - relTopLeft[1] * sinAngle;
            rotated[1] =
                relTopLeft[0] * sinAngle + relTopLeft[1] * cosAngle;
            points[1][0] = rotated[0] + center[0];
            points[1][1] = rotated[1] + center[1];

            // topRight (points[2])
            rotated[0] =
                relTopRight[0] * cosAngle - relTopRight[1] * sinAngle;
            rotated[1] =
                relTopRight[0] * sinAngle + relTopRight[1] * cosAngle;
            points[2][0] = rotated[0] + center[0];
            points[2][1] = rotated[1] + center[1];

            // bottomRight (points[3])
            rotated[0] = relBottomRight[0] * cosAngle -
                         relBottomRight[1] * sinAngle;
            rotated[1] = relBottomRight[0] * sinAngle +
                         relBottomRight[1] * cosAngle;
            points[3][0] = rotated[0] + center[0];
            points[3][1] = rotated[1] + center[1];

            // Render the lines
            mat4 view;
            smCamera_GetViewMatrix(&smState.camera, view);
            smRenderer_RenderLine2D(
                points, 4, (vec4) {0.0f, 1.0f, 0.0f, 1.0}, 10.0f,
                3.0f, true, smState.persProj, view);
        }
        else if (col->colliderType == sm2d_Polygon)
        {
            if (col->points == NULL)
            {
                col->points = smVector_Create(sizeof(vec2), 10);
            }

            vec2 points[32] = {};
            int  pointCount = 0;
            for (int i = 0; i < col->points->size; ++i)
            {
                vec2* point = (vec2*)smVector_Get(col->points, i);

                // Get the rotation angle from transform
                float rotation =
                    col->body->transform->rotation[2]; // in radians

                // First create rotated point (before translation)
                float cosAngle = cos(rotation);
                float sinAngle = sin(rotation);

                // First rotate
                vec2 rotatedPoint = {
                    (*point)[0] * cosAngle - (*point)[1] * sinAngle,
                    (*point)[0] * sinAngle + (*point)[1] * cosAngle};

                // Then translate
                glm_vec2_add(
                    rotatedPoint,
                    (vec2) {col->body->transform->position[0],
                            col->body->transform->position[1]},
                    points[i]); // Store directly in the output array

                pointCount = i;
            }

            // Render the lines
            mat4 view;
            smCamera_GetViewMatrix(&smState.camera, view);
            smRenderer_RenderLine2D(points, pointCount + 1,
                                    (vec4) {0.0f, 1.0f, 0.0f, 1.0},
                                    10.0f, 3.0f, true,
                                    smState.persProj, view);
        }
    }
    SM_ECS_ITER_END();
}

void smRigidbody2DDraw(smRigidbody2D* rb)
{
    if (smImGui_CollapsingHeader("Rigidbody2D"))
    {
        const char* types[] = {"Static", "Dynamic", "Kinematic"};
        int         currentType = (int)rb->type;
        if (smImGui_ComboBox("rg2d Body Type", types, &currentType,
                             3))
        {
            rb->type = (smRigidbody2DType)currentType;
        }

        smImGui_DragFloat("sm2d Mass", &rb->mass, 0.1f);
        smImGui_DragFloat("sm2d Friction", &rb->friction, 0.1f);
        smImGui_DragFloat("sm2d Linear Damping", &rb->linearDamping,
                          0.1f);
        smImGui_DragFloat("sm2d Angular Damping", &rb->angularDamping,
                          0.1f);
        smImGui_DragFloat("sm2d Restitution", &rb->restitution, 0.1f);
        smImGui_Checkbox("sm2d Fixed Rotation", &rb->fixedRotation);
        smImGui_Checkbox("sm2d Always Awake", &rb->alwaysAwake);
    }
}

smJson smRigidbody2DSave(smRigidbody2D* rb)
{
    smJson j = smJson_Create();

    smJson_SaveFloat(j, "Mass", rb->mass);
    smJson_SaveFloat(j, "LinearDamping", rb->linearDamping);
    smJson_SaveFloat(j, "AngularDamping", rb->angularDamping);
    smJson_SaveFloat(j, "Restitution", rb->restitution);
    smJson_SaveBool(j, "FixedRotation", rb->fixedRotation);
    smJson_SaveBool(j, "AlwaysAwake", rb->alwaysAwake);
    smJson_SaveFloat(j, "Friction", rb->friction);
    smJson_SaveInt(j, "Type", rb->type);

    return j;
}

void smRigidbody2DLoad(smRigidbody2D* rb, smJson j)
{
    smJson_LoadFloat(j, "Mass", &rb->mass);
    smJson_LoadFloat(j, "LinearDamping", &rb->linearDamping);
    smJson_LoadFloat(j, "AngularDamping", &rb->angularDamping);
    smJson_LoadFloat(j, "Restitution", &rb->restitution);
    smJson_LoadBool(j, "FixedRotation", &rb->fixedRotation);
    smJson_LoadBool(j, "AlwaysAwake", &rb->alwaysAwake);
    smJson_LoadFloat(j, "Friction", &rb->friction);
    smJson_LoadInt(j, "Type", &rb->type);
}

float pdragThreshold = 0.3f;
int   pdragIndex = -1;

void smCollider2DDraw(smCollider2D* col)
{
    if (smImGui_CollapsingHeader("Collider2D"))
    {
        int colTypeValue = (int)col->colliderType;
        if (smImGui_SliderInt("sm2d ColliderType2D", &colTypeValue, 0,
                              2))
        {
            col->colliderType = colTypeValue;
        }

        smImGui_InputHex("sm2d CategoryBits", &col->categoryBits);
        smImGui_InputHex("sm2d MaskBits", &col->maskBits);

        switch (col->colliderType)
        {
            case sm2d_Box:
                smImGui_DragFloat2("sm2d Half Widths",
                                   col->halfwidths, 0.1f);
                break;

            case sm2d_Circle:
                smImGui_DragFloat("sm2d Radius", &col->radius, 0.1f);
                break;

            case sm2d_Polygon:
                smImGui_DragFloat("sm2d Drag threshold for points",
                                  &pdragThreshold, 0.1f);

                if (col->points == NULL)
                {
                    col->points = smVector_Create(sizeof(vec2), 10);
                }

                if (col->body == NULL)
                {
                    SM_ECS_ITER_START(
                        smState.scene,
                        SM_ECS_COMPONENT_TYPE(smCollider2D))
                    {
                        smRigidbody2D* rigid = SM_ECS_GET(
                            smState.scene, _entity, smRigidbody2D);
                        smCollider2D* col = SM_ECS_GET(
                            smState.scene, _entity, smCollider2D);
                        smTransform* trans = SM_ECS_GET(
                            smState.scene, _entity, smTransform);

                        col->body = rigid;
                        col->body->transform = trans;
                        rigid->transform = trans;
                    }
                    SM_ECS_ITER_END();
                }

                vec2 oldMousePos;
                oldMousePos[0] = smInput_GetMouseInputHorizontal();
                oldMousePos[1] = smInput_GetMouseInputVertical();
                vec2 mousePos = GLM_VEC2_ZERO_INIT;
                smCamera_ScreenToWorld2D(&smState.camera, oldMousePos,
                                         mousePos);

                bool mouse =
                    smInput_GetMouseButton(SM_MOUSE_BUTTON_LEFT);
                bool hovering = false;

                mat4 view;
                smCamera_GetViewMatrix(&smState.camera, view);
                smRenderer_RenderLine2D(
                    &mousePos, 1, (vec4) {1.0f, 0.0f, 0.0f, 1.0},
                    10.0f, 3.0f, true, smState.persProj, view);

                for (int i = 0; i < col->points->size; i++)
                {
                    char label[50] = "Point ";
                    sprintf(label, "%d", i);
                    vec2* pointPtr =
                        (vec2*)smVector_Get(col->points, i);

                    smImGui_DragFloat2(label, (*pointPtr), 0.1f);

                    vec2 transformedPoint = GLM_VEC2_ZERO_INIT;
                    glm_vec2_add(
                        (*pointPtr),
                        (vec2) {col->body->transform->position[0],
                                col->body->transform->position[1]},
                        transformedPoint);

                    float distance =
                        glm_vec2_distance(mousePos, transformedPoint);

                    if (distance <= pdragThreshold && !mouse)
                    {
                        pdragIndex = i;
                        hovering = true;
                    }
                }

                if (!mouse && !hovering)
                {
                    pdragIndex = -1;
                }

                if (mouse && pdragIndex != -1)
                {
                    vec2* vec =
                        (vec2*)smVector_Get(col->points, pdragIndex);
                    (*vec)[0] = mousePos[0] -
                                col->body->transform->position[0];
                    (*vec)[1] = mousePos[1] -
                                col->body->transform->position[1];
                }

                if (smImGui_Button("rg2d Add Point"))
                {
                    vec2 defaultPoint = GLM_VEC2_ZERO_INIT;
                    smVector_PushBack(col->points, &defaultPoint);
                }
                break;
        }
    }
}

smJson smCollider2DSave(smCollider2D* col)
{
    smJson j = smJson_Create();

    smJson_SaveInt(j, "Type", col->colliderType);
    smJson_SaveInt(j, "CategoryBits", col->categoryBits);
    smJson_SaveInt(j, "MaskBits", col->maskBits);

    switch (col->colliderType)
    {
        case sm2d_Box:
            smJson_SaveVec2(j, "HalfWidths", col->halfwidths);
            break;

        case sm2d_Circle:
            smJson_SaveFloat(j, "Radius", col->radius);
            break;

        case sm2d_Polygon:
            float points[64] = {};
            int   pointIndex = 0;
            for (int i = 0; i < col->points->size; i++)
            {
                vec2* point = (vec2*)smVector_Get(col->points, i);

                points[pointIndex] = (*point)[0];
                points[pointIndex + 1] = (*point)[1];
                pointIndex += 2;
            }
            smJson_SaveFloatArray(j, "Points", points,
                                  col->points->size * 2);
            break;
    }

    return j;
}

void smCollider2DLoad(smCollider2D* col, smJson j)
{
    smJson_LoadInt(j, "Type", &col->colliderType);
    smJson_LoadInt(j, "CategoryBits", &col->categoryBits);
    smJson_LoadInt(j, "MaskBits", &col->maskBits);

    switch (col->colliderType)
    {
        case sm2d_Box:
            smJson_LoadVec2(j, "HalfWidths", col->halfwidths);
            break;

        case sm2d_Circle:
            smJson_LoadFloat(j, "Radius", &col->radius);
            break;

        case sm2d_Polygon:
            float  points[64] = {};
            size_t size = smJson_LoadFloatArray(j, "Points", points);

            if (col->points == NULL)
            {
                col->points = smVector_Create(sizeof(vec2), 10);
            }

            // assert(size % 2 != 0);

            for (int i = 1; i < size; i += 2)
            {
                vec2 point = {points[i - 1], points[i]};
                smVector_PushBack(col->points, &point);
            }
            break;
    }
}
