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

    vec3 groundNormal;

    float gravityTimer;

    int   health;
    float healthTimer; // Time it takes to get hurt again

    unsigned int fullHeartSprite;
    unsigned int emptyHeartSprite;

    smImage* heartImages[3];
    vec2     heartSpriteScale;

    smSound slideSound;
    smSound hurtSound;
    char    slideSoundPath[128];
    char    hurtSoundPath[128];

    bool hurt;
    int  hurtDamage;

    smAudioSource audioSource;
    smAudioSource slideAudioSource;

    PlayerState state;
} Player;

typedef struct
{
    int health;
} PlayerData;

extern PlayerData playerData;

void   Player_Draw(Player* player);
smJson Player_Save(Player* player);
void   Player_Load(Player* player, smJson j);

void Player_StartSys();
void Player_Sys();
