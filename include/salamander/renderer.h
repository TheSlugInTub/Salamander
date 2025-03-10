#pragma once

#include <salamander/shader.h>

extern unsigned int sm_VAO, sm_VBO,
    sm_EBO;                  // OpenGL buffer objects for 2d
extern smShader sm_shader2d; // 2D shader

extern unsigned int sm_linesVAO2d, sm_linesVBO2d, sm_linesEBO2d;
extern smShader     sm_linesShader2d; // Lines shader

extern unsigned int sm_linesVAO, sm_linesVBO, sm_linesEBO;
extern smShader     sm_linesShader3d; // Lines shader

extern smShader sm_shader3d; // 3D shader

void smRenderer_InitShaders();
void smRenderer_Init2D();
void smRenderer_InitLines();

void smRenderer_RenderQuad(vec3 position, float rotation, vec2 scale,
                           unsigned int texture, vec4 color,
                           mat4 projection, mat4 view);

void smRenderer_RenderLine2D(vec2* lines, int lineCount, vec4 color,
                             float pointSize, float lineSize,
                             bool looping, mat4 projection, mat4 view);
