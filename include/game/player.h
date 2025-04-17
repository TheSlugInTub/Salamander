#pragma once

#include <salamander/components.h>
#include <salamander/json_api.h>
#include <salamander/physics_3d.h>
#include <salamander/ui_image.h>
#include <salamander/ecs_api.h>
#include <salamander/audio.h>

typedef enum
{
    PlayerState_Walking,
    PlayerState_Flying,
    PlayerState_Wallrunning
} PlayerState;

typedef struct
{
    smTransform*   trans;
    smRigidbody3D* rigid;
    bool           grounded;
    bool           crouched;

    float speed;
    float slideSpeed;
    float acceleration;
    float deceleration;

    float airSpeed;
    float airAcceleration;

    float jumpSpeed;
    float dashSpeed;

    float walkDashSpeed;

    vec3 groundNormal;

    float leafRegenSpeed;
    int   currentLeafCount;

    float leafRegenTimer;

    float walkDashAirTime;
    float gravityTimer;

    int health;

    unsigned int fullLeafSprite;
    unsigned int emptyLeafSprite;

    smImage* leafImages[10];
    vec2     leafSpriteScale;

    unsigned int fullHeartSprite;
    unsigned int emptyHeartSprite;

    smImage* heartImages[3];
    vec2     heartSpriteScale;

    smSound leafThrowSound;
    smSound leafDashSound;
    smSound dashSound;
    smSound slideSound;
    char    leafThrowSoundPath[128];
    char    leafDashSoundPath[128];
    char    dashSoundPath[128];
    char    slideSoundPath[128];

    smAudioSource audioSource;
    smAudioSource slideAudioSource;

    PlayerState state;
} Player;

typedef struct
{
    int leafCount;
    int health;
} PlayerData;

extern PlayerData playerData;

void   Player_Draw(Player* player);
smJson Player_Save(Player* player);
void   Player_Load(Player* player, smJson j);

void Player_StartSys();
void Player_Sys();
