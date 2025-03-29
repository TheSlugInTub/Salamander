#pragma once

#include <salamander/components.h>
#include <salamander/json_api.h>
#include <salamander/physics_3d.h>

typedef struct 
{
    smTransform* trans;
    smRigidbody3D* rigid;
    float moveSpeed;
    float jumpSpeed;
    float leafSpeed;
    float dashSpeed;
    bool grounded;
} Player;

void Player_Draw(Player* player);
smJson Player_Save(Player* player);
void Player_Load(Player* player, smJson j);

void Player_StartSys();
void Player_Sys();
