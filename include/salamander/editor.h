#pragma once

#include <salamander/state.h>

extern int        sm_selectedEntityIndex;
extern smEntityID sm_selectedEntity;
extern char       sm_sceneName[100];
extern bool       sm_playing;

void smEditor_DrawHierarchy();
void smEditor_DrawInspector();
void smEditor_DrawTray();
