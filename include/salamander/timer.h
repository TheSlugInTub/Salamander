#pragma once

typedef struct 
{ 
    double startTime;
} smTimer;

void smTimer_Start(smTimer* timer);
void smTimer_PrintSeconds(smTimer timer);
