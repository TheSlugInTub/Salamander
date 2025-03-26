#pragma once

#include <salamander/glm.h>
#include <salamander/components.h>
#include <salamander/json_api.h>

typedef struct 
{
    smTransform* trans;
    float moveSpeed;
    float jumpSpeed;
} Player;

void Player_Draw(Player* player);
smJson Player_Save(Player* player);
void Player_Load(Player* player, smJson j);

void Player_StartSys();
void Player_Sys();
