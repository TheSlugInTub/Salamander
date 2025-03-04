#pragma once

#include <salamander/glm.h>

typedef struct
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
} smTransform;

typedef struct 
{
    const char* name;
} Name;

typedef struct 
{
    unsigned int texture;
    const char* texturePath;
    vec4 color;
} smSpriteRenderer;
