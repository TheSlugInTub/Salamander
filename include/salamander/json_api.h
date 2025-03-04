#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#include <stddef.h>
#include <stdbool.h>
#include <salamander/glm.h>

typedef struct Json_t* Json;
typedef struct JsonIterator_t JsonIterator;

Json Json_Create();
void Json_Destroy(Json j);
void Json_CreateArray(Json j);

void Json_SaveBool(Json j,   const char* name, const bool val);
void Json_SaveString(Json j, const char* name, const char* val);
void Json_SaveInt(Json j,    const char* name, const int val);
void Json_SaveFloat(Json j,  const char* name, const float val);
void Json_SaveDouble(Json j, const char* name, const double val);

void Json_SaveVec2(Json j, const char* name, const vec2 val);
void Json_SaveVec3(Json j, const char* name, const vec3 val);
void Json_SaveVec4(Json j, const char* name, const vec4 val);

void Json_SaveMat4(Json j, const char* name, const mat4 val);

void Json_LoadBool(Json j,   const char* key, bool* val);
void Json_LoadString(Json j, const char* key, char* val);
void Json_LoadInt(Json j,    const char* key, int* val);
void Json_LoadFloat(Json j,  const char* key, float* val);
void Json_LoadDouble(Json j, const char* key, double* val);

void Json_LoadVec2(Json j, const char* key, vec2 val);
void Json_LoadVec3(Json j, const char* key, vec3 val);
void Json_LoadVec4(Json j, const char* key, vec4 val);

void Json_LoadMat4(Json j, const char* key, mat4 val);

void Json_PushBack(Json j, const Json val);

typedef void (*JsonIteratorFunc)(Json j);

void Json_Iterate(Json j, JsonIteratorFunc sys);

Json Json_GetJsonAtIndex(Json j, int index);
int Json_GetJsonArraySize(Json j);
bool Json_HasKey(Json j, const char* key);

#ifdef __cplusplus 

}

#include <json.hpp>

nlohmann::json Json_GetJson(Json j);

void Json_SetJson(const Json j, const nlohmann::json& json);

#endif
