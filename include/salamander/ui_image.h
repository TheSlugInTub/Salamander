#pragma once

#include <salamander/glm.h>
#include <salamander/json_api.h>

typedef struct
{
    char         texturePath[256];
    unsigned int texture;

    vec4 color;
    vec2 position;
    vec2 scale;
} smImage;

void smImage_StartSys();
void smImage_Sys();

void   smImage_Draw(smImage* image);
smJson smImage_Save(smImage* image);
void   smImage_Load(smImage* image, smJson j);
