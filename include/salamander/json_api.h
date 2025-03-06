#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>
#include <salamander/glm.h>

typedef struct smJson_t*        smJson;
typedef struct smJsonIterator_t smJsonIterator;

smJson smJson_Create();
void   smJson_Destroy(smJson j);
void   smJson_CreateArray(smJson j);

void smJson_SaveBool(smJson j, const char* name, const bool val);
void smJson_SaveString(smJson j, const char* name, const char* val);
void smJson_SaveInt(smJson j, const char* name, const int val);
void smJson_SaveFloat(smJson j, const char* name, const float val);
void smJson_SaveDouble(smJson j, const char* name, const double val);

void smJson_SaveVec2(smJson j, const char* name, const vec2 val);
void smJson_SaveVec3(smJson j, const char* name, const vec3 val);
void smJson_SaveVec4(smJson j, const char* name, const vec4 val);

void smJson_SaveMat4(smJson j, const char* name, const mat4 val);

void smJson_LoadBool(smJson j, const char* key, bool* val);
void smJson_LoadString(smJson j, const char* key, char* val);
void smJson_LoadInt(smJson j, const char* key, int* val);
void smJson_LoadFloat(smJson j, const char* key, float* val);
void smJson_LoadDouble(smJson j, const char* key, double* val);

void smJson_LoadVec2(smJson j, const char* key, vec2 val);
void smJson_LoadVec3(smJson j, const char* key, vec3 val);
void smJson_LoadVec4(smJson j, const char* key, vec4 val);

void smJson_LoadMat4(smJson j, const char* key, mat4 val);

void smJson_PushBack(smJson j, const smJson val);

typedef void (*smJsonIteratorFunc)(smJson j);

void smJson_Iterate(smJson j, smJsonIteratorFunc sys);

smJson smJson_GetsmJsonAtIndex(smJson j, int index);
int    smJson_GetsmJsonArraySize(smJson j);
bool   smJson_HasKey(smJson j, const char* key);

bool   smJson_SaveToFile(smJson j, const char* filename);
smJson smJson_LoadFromFile(const char* filename);

#ifdef __cplusplus
}

#    include <json.hpp>

nlohmann::json smJson_GetsmJson(smJson j);

void smJson_SetsmJson(const smJson j, const nlohmann::json& json);

#endif
