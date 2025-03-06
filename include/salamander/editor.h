#pragma once

#include <salamander/state.h>

extern int smSelectedEntityIndex;
extern EntityID smSelectedEntity;
extern char smSceneName[100];
extern bool smPlaying;

void smEditor_DrawHierarchy();
void smEditor_DrawInspector();
void smEditor_DrawTray();