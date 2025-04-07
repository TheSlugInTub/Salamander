#pragma once

#include <salamander/glm.h>
#include <salamander/json_api.h>
#include <salamander/umap.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct 
{
    unsigned int textureID;
    vec2 size;
    vec2 bearing;
    unsigned int advance;
} smCharacter;

typedef struct
{
    char fontPath[256];
    vec4 color;
    vec2 position;
    vec2 scale;
    float leading;
    char text[4096];
} smText;

extern FT_Library sm_fontLibrary;
extern smUmap* sm_fonts; // umap<const char*, umap<char, smCharacter>>

void smText_StartSys();
void smText_Sys();

void   smText_Draw(smText* text);
smJson smText_Save(smText* text);
void   smText_Load(smText* text, smJson j);

void smLoadFont(const char* fontPath, int pixelSize);
