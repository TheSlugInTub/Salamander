#pragma once

#include <cglm/cglm.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void*                    smImGuiTextureID;
typedef struct smImGuiPayload_t* smImGuiPayload;

void smImGui_Init(struct GLFWwindow* window);
void smImGui_NewFrame();
void smImGui_EndFrame();
void smImGui_Terminate();
void smImGui_Theme1();

bool smImGui_Begin(const char* name);
void smImGui_End();

bool smImGui_DragFloat(const char* name, float* val, float speed);
bool smImGui_DragFloat2(const char* name, float* val, float speed);
bool smImGui_DragFloat3(const char* name, float* val, float speed);
bool smImGui_DragFloat4(const char* name, float* val, float speed);

bool smImGui_ComboBox(const char* name, const char** types,
                      int currentType, int typeSize);

bool smImGui_Checkbox(const char* name, bool* val);
bool smImGui_SliderInt(const char* name, int* currentType, int min,
                       int max);
bool smImGui_Button(const char* name);
bool smImGui_ImageButton(smImGuiTextureID tex, vec2 size);
bool smImGui_InputText(const char* name, char* buffer, size_t size,
                       int flags);

bool smImGui_IsWindowHovered();
bool smImGui_CollapsingHeader(const char* name);
bool smImGui_ColorPicker(const char* name, vec4 color);
void smImGui_Textf(const char* val, ...);
void smImGui_Text(const char* val);

bool smImGui_MenuItem(const char* name);
void smImGui_PushID(int id);
void smImGui_PopID();
bool smImGui_Selectable(const char* name, bool selected);
bool smImGui_BeginDragDropSource(int flags);
void smImGui_SetDragDropPayload(const char* name, const void* data,
                                size_t size);
void smImGui_EndDragDropSource();

bool smImGui_BeginDragDropTarget();
void smImGui_EndDragDropTarget();

void smImGui_Separator();

smImGuiPayload smImGui_AcceptDragDropPayload(const char* name);
void*          smImGuiPayload_GetData(smImGuiPayload payload);
int            smImGuiPayload_GetDataSize(smImGuiPayload payload);

bool smImGui_BeginPopupContextWindow();
void smImGui_EndPopup();

#ifdef __cplusplus
}
#endif
