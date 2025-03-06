#pragma once

#include <cglm/cglm.h>

typedef struct
{
    unsigned int ID;
} smShader;

smShader smShader_Create(const char* vertexShader,
                         const char* fragmentShader);

smShader smShader_CreateGeometry(const char* vertexShader,
                                 const char* fragmentShader,
                                 const char* geometryShader);

void smShader_Use(smShader shader);

void smShader_SetBool(smShader shader, const char* name, const bool value);
void smShader_SetInt(smShader shader, const char* name, const int value);
void smShader_SetFloat(smShader shader, const char* name,
                       const float value);
void smShader_SetVec2(smShader shader, const char* name, const vec2 value);
void smShader_SetVec3(smShader shader, const char* name, const vec3 value);
void smShader_SetVec4(smShader shader, const char* name, const vec4 value);
void smShader_SetMat2(smShader shader, const char* name, const mat2 value);
void smShader_SetMat3(smShader shader, const char* name, const mat3 value);
void smShader_SetMat4(smShader shader, const char* name, const mat4 value);
void smShader_SetTexture2D(smShader shader, const char* name,
                           const unsigned int value, int sampler);

void smShader_CheckCompileErrors(unsigned int ID, const char* type);
