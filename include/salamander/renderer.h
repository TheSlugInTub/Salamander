#pragma once

#include <salamander/shader.h>

extern unsigned int smVAO, smVBO,
    smEBO;                  // OpenGL buffer objects for 2d
extern smShader smShader2d; // 2D shader

extern unsigned int smLinesVAO2d, smLinesVBO2d, smLinesEBO2d;
extern smShader     smShaderLines2d; // Lines shader

extern unsigned int smLinesVAO, smLinesVBO, smLineEBO;
extern smShader     smShaderLines3d; // Lines shader

typedef struct
{
    vec3         position;
    float        rotation;
    vec2         scale;
    unsigned int texture;
    vec4         color;
    mat4         projection;
    mat4         view;
} smQuad;

void smRenderer_InitShaders();
void smRenderer_Init2D();
void smRenderer_RenderQuad(const smQuad* quad);
