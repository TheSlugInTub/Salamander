#pragma once

#include <salamander/glm.h>

typedef struct
{
    vec3 position;
    quat rotation;
    vec3 scale;
} smTransform;

typedef struct
{
    char name[128];
} smName;

typedef struct
{
    unsigned int texture;
    char         texturePath[128];
    vec4         color;
} smSpriteRenderer;
