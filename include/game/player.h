#pragma once

#include <salamander/components.h>
#include <salamander/json_api.h>
#include <salamander/physics_3d.h>

typedef enum
{
    PlayerState_Walking,
    PlayerState_Flying,
    PlayerState_Wallrunning
} PlayerState;

typedef struct 
{
    smTransform* trans;
    smRigidbody3D* rigid;
    bool grounded;
    bool crouched;

    float speed;
    float slideSpeed;
    float acceleration;
    float deceleration;

    float airSpeed;
    float airAcceleration;

    float jumpSpeed;
    float dashSpeed;

    vec3 groundNormal;

    PlayerState state;
} Player;

void Player_Draw(Player* player);
smJson Player_Save(Player* player);
void Player_Load(Player* player, smJson j);

void Player_StartSys();
void Player_Sys();
